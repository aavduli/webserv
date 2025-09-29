#include "MessageValidator.hpp"
#include <cstring>

// Host header format: "hostname:port" or just "hostname"
bool	is_valid_host(RequestUri *uri, const std::vector<std::string>& header_host, const std::string& config_host) {

	if (uri->getHost().empty() && !header_host.empty()) {
		std::string tmp_host = header_host.at(0);
		size_t colon = tmp_host.find(":");
		if (colon != std::string::npos)
			uri->setHost(tmp_host.substr(0, colon));
		else
			uri->setHost(tmp_host);
	}
	if (!(uri->getHost()).empty() && (uri->getHost()).compare(config_host)) {
		console::log("[ERROR] Invalid host: " + (uri->getHost()), MSG);
		console::log("Expected server host: " + config_host, MSG);
		return false;
	}
	else if ((uri->getHost()).empty())
		uri->setHost(config_host);
	return true;
}

// Host header format: "hostname:port"
bool	is_valid_port(RequestUri *uri, const std::vector<std::string>& header_port, const std::string& config_port) {

	if (uri->getPort().empty() && !header_port.empty()) {
		std::string tmp_port = header_port.at(0);
		size_t colon = tmp_port.find(":");
		if (colon != std::string::npos)
			uri->setPort(tmp_port.substr(colon + 1));
	}
	// Allow default HTTP port (80) or configured port
	if (!(uri->getPort()).empty() && (uri->getPort()) != "80" && (uri->getPort()).compare(config_port)) {
		console::log("[ERROR] Invalid port: " + (uri->getPort()), MSG);
		console::log("Expected server port: " + config_port, MSG);
		return false;
	}
	else if ((uri->getPort()).empty())
		uri->setPort(config_port);
	return true;
}

bool	is_allowed_method(const std::string& method, std::map<std::string, std::string> config_loc) {

	if (config_loc.empty()) {
		console::log("[INFO] No location config found", MSG);
		if (method == "GET" || method == "POST" || method == "DELETE")
			return true;
	}
	else {
		std::string allowed_methods = config_loc["methods"];
		std::vector<std::string> methods = str_to_vect(allowed_methods, " ");
		std::vector<std::string>::iterator it;
		for (it = methods.begin(); it != methods.end(); it++) {
			if (method == *it)
				return true;
		}
	}
	console::log("[ERROR] Invalid method " + method, MSG);
	// Should return 405 Method Not Allowed
	return false;
}

// need default max MACRO?
bool	is_valid_body_size(const size_t& size, const std::string& config_max) {

	if (!config_max.empty()) {
		size_t max = to_size_t(config_max);
		if (size > max) {
			console::log("[ERROR] Content-Length value > client_max_body_size", MSG);
			return false;
		}
	}
	return true;
}

// traversal = use of ../ sequences (or other patterns) to escape document root
bool contains_traversal(const std::string& path) {

	return path.find("../") != std::string::npos ||
		path.find("..\\") != std::string::npos ||
		path.find("%2e%2e") != std::string::npos ||
		path.find("..%2f") != std::string::npos;
}

// Canonicalization by resolving all symbolic links, redundant components (. / .. etc.) and normalizing path structure
// Ensures that paths are in standard form for security validation
std::string canonicalize_path(const std::string& path) {

	std::vector<std::string> parts;
	parts = str_to_vect(path, "/");
	
	std::vector<std::string> canonical;
	for (size_t i = 0; i < parts.size(); ++i) {
		if (parts[i].empty() || parts[i] == ".")		// ignore empty and current dir
			continue;
		else if (parts[i] == "..") {					// go backwards if possible
			if (!canonical.empty())
				canonical.pop_back();
		}
		else
			canonical.push_back(parts[i]);
	}
	
	std::string canonical_path;
	bool is_absolute = (!path.empty() && path[0] == '/');
	if (is_absolute)
		canonical_path = "/";
	
	for (size_t i = 0; i < canonical.size(); ++i) {
		if (i > 0 || (is_absolute && !canonical_path.empty() && canonical_path != "/"))
			canonical_path += "/";
		canonical_path += canonical[i];
	}
	if (canonical_path.empty())
		canonical_path = is_absolute ? "/" : ".";
		
	return canonical_path;
}

bool is_within_root(const std::string& resolved_path, const std::string& document_root) {

	std::string canonical_root = canonicalize_path(document_root);
	std::string canonical_path = canonicalize_path(resolved_path);
	
	if (canonical_root[canonical_root.length() - 1] != '/')
		canonical_root += "/";
	
	if (canonical_path.substr(0, canonical_root.length()) == canonical_root)
		return true;
	else if (canonical_path == canonical_root.substr(0, canonical_root.length() - 1))
		return true;
	return false;
}

bool	is_valid_path(RequestUri *uri, const WebservConfig& config, std::map<std::string, std::string> location) {

	std::string path = uri->getPath();
	std::string final_path;
	
	if (contains_traversal(path)) {
		console::log("[ERROR] Path contains directory traversal attempt", MSG);
		return false;
	}

	std::string root = location["root"];
	if (root.empty())
		root = config.getDirective("root");
	final_path = root + path;
	std::string canonical_path = canonicalize_path(final_path);
	console::log("[INFO] Canonical path: " + canonical_path, MSG);

	struct stat buf;
	if (stat(canonical_path.c_str(), &buf) != 0) {
		console::log("[ERROR] File not found: " + canonical_path + " - " + strerror(errno), MSG);
		return false;
	}
	else if (S_ISDIR(buf.st_mode)) {
		std::string index = location["index"];
		if (index.empty())
			index = config.getDirective("index");
		if (canonical_path[canonical_path.length() - 1] != '/') {
			canonical_path += "/";
		}
		canonical_path += index;
		if (stat(canonical_path.c_str(), &buf) != 0) {
			console::log("[ERROR] Index file not found: " + canonical_path, MSG);
			return false;
		}
	}

	if (!is_within_root(canonical_path, root)) {
		console::log("[ERROR] Final path escapes document root: " + canonical_path, MSG);
		return false;
	}

	uri->setFullPath(canonical_path);
	console::log("[INFO] Validated path: " + canonical_path, MSG);
	return true;
}