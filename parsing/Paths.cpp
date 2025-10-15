#include "Parsing.hpp"
#include "../console/console.hpp"

bool is_valid_path(const std::string& path) {
	
	if (path.empty())
		return false;

	struct stat buf;
	return stat(path.c_str(), &buf) == 0;
}

bool is_valid_file_path(const std::string& path) {
	
	if (path.empty())
		return false;

	struct stat buf;
	return stat(path.c_str(), &buf) == 0 && S_ISREG(buf.st_mode);
}

bool is_directory(const std::string& path) {

	if (path.empty())
		return false;

	struct stat buf;
	return stat(path.c_str(), &buf) == 0 && S_ISDIR(buf.st_mode);
}

std::string get_file_extension(const std::string& path) {

	size_t	dot = path.find_last_of(".");
	if (dot == std::string::npos || dot == path.length() - 1)
		return "";
	
	std::string extension = path.substr(dot + 1);
	return (extension);
}

std::string	remove_prefix(const std::string& path, const std::string& prefix) {

	std::string relative_path = path;

	if (!prefix.empty() && path.find(prefix) == 0)
		relative_path = path.substr(prefix.length());
	return relative_path;
}

std::string build_full_path(const std::string& root_path, const std::string& relative_path) {

	std::string path = relative_path;
	std::string root = root_path;
	
	// ensure terminating /
	if (!root_path.empty() && root_path[root_path.length() - 1] != '/')
		root += "/";

	// remove leading /
	if (!relative_path.empty() && relative_path[0] == '/')
		path = relative_path.substr(1);

	return root + path;
}

// traversal = use of ../ sequences (or other patterns) to escape document root
bool contains_traversal(const std::string& path) {

	return path.find("../") != std::string::npos ||
		path.find("..\\") != std::string::npos ||
		path.find("%2e%2e") != std::string::npos ||
		path.find("..%2f") != std::string::npos;
}

// ensure paths are normalized for security validation (symbolic links, . / ..)
std::string canonicalize_path(const std::string& path) {

	std::vector<std::string> parts = str_to_vect(path, "/");
	std::vector<std::string> canonical;
	std::string canonical_path;
	bool is_absolute;

	for (size_t i = 0; i < parts.size(); ++i) {
		if (parts[i].empty() || parts[i] == ".")
			continue;
		else if (parts[i] == ".." && !canonical.empty())
			canonical.pop_back();
		else
			canonical.push_back(parts[i]);
	}
	
	is_absolute = (!path.empty() && path[0] == '/');
	if (is_absolute)
		canonical_path = "/";

	for (size_t i = 0; i < canonical.size(); ++i) {
		if (i > 0 || (is_absolute && !canonical_path.empty() && canonical_path != "/"))
			canonical_path += "/";
		canonical_path += canonical[i];
	}
	if (canonical_path.empty() && !is_absolute)
		canonical_path = ".";
	return canonical_path;
}

// ensure path doesn't escape root
bool is_within_root(const std::string& path, const std::string& document_root) {

	std::string canonical_root = canonicalize_path(document_root);
	std::string canonical_path = canonicalize_path(path);
	
	if (canonical_root[canonical_root.length() - 1] != '/')
		canonical_root += "/";
	if (canonical_path.substr(0, canonical_root.length()) == canonical_root)
		return true;
	else if (canonical_path == canonical_root.substr(0, canonical_root.length() - 1))
		return true;
	return false;
}
