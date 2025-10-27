/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebservConfig.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jim <jim@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/08 16:37:53 by jim               #+#    #+#             */
/*   Updated: 2025/09/15 14:16:38 by jim              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebservConfig.hpp"
#include "../console/console.hpp"
#include <cstdlib>
#include <string>
#include <sstream>
#include "ParsingUtils.hpp"
#include <sstream>
#include <cstdlib>
#include <cerrno>

WebservConfig::WebservConfig(): _isValid(false){}

WebservConfig::WebservConfig(const std::string& configFile):
	_configFile(configFile), _isValid(false)
{
	console::log("config file = " +  _configFile, CONF);
	loadConfig(configFile);
}

WebservConfig::~WebservConfig(){}

bool WebservConfig::isValid() const{
	return _isValid;
}

//getServer
const std::map<std::string, std::string>& WebservConfig::getServer() const{
	return _server;
}


//getAllLocation
const std::map<std::string, std::map<std::string, std::string> >& WebservConfig::getAllLocations() const{
	return _locations;
}

std::string WebservConfig::getDirective(const std::string& directive) const {
	if (_servers.empty()) return "";
	std::map<std::string, std::string>::const_iterator it =_servers[0].directives.find(directive);
	return (it != _servers[0].directives.end()) ? it->second : "";
}

std::string WebservConfig::getConfigFile() const{
	return _configFile;
}

//getDirective
std::string WebservConfig::getDirective(const std::string& directive)const{
	std::map<std::string, std::string>::const_iterator it = _server.find(directive);
	return (it != _server.end()) ? it->second : "";
}

bool WebservConfig::loadConfig(const std::string& configFile){
	_configFile = configFile;
	FileReader reader;
	ConfigParser parser;

	std::vector<std::string> lines = reader.readLines(configFile);
	if (lines.empty()) return false;

	if (!_validator.validateSyntax(lines)){
		console::log("Syntax error: " + _validator.getLastError(), CONF);
		return false;
	}

	ServerConfig serverConfig = parser.parseServer(lines);
	LocationsConfig locationsConfig = parser.parseLocations(lines);

	if (!_validator.validateServerConfig(serverConfig)){
		console::log("Server config error: " + _validator.getLastError(), CONF);
		return false;
	}

	for (std::map<std::string, LocationConfig>::const_iterator it = locationsConfig.locations.begin();
		it != locationsConfig.locations.end(); ++it){
			if (!_validator.validateLocationConfig(it->second)){
				console::log("Location " + it->first + " config error: " + _validator.getLastError(), CONF);
				return false;
			}
		}

	_server = serverConfig.directives;

	//adding multpiple ports
	_listen_ports.clear();

	for (size_t i = 0; i < serverConfig.listen_ports.size(); i++){
		std::string listen = serverConfig.listen_ports[i];
		std::vector<std::string> parts = _utils.split(listen, ':');

		char* endptr;
		long port;

		if (parts.size() == 2){
			port = std::strtol(parts[1].c_str(), &endptr, 10); //foarmat "host:port"
		}else{
			port = std::strtol(listen.c_str(), &endptr, 10);//format "port"
		}

		if (port > 0 && port <= 65535){
			_listen_ports.push_back(static_cast<int>(port));
		}
	}

	_locations = convertToOldFormat(locationsConfig);
	_isValid = true;
	return _isValid;
}

std::map<std::string, std::map<std::string, std::string> > WebservConfig::convertToOldFormat(const LocationsConfig& locationsConfig) const {
	std::map<std::string, std::map<std::string, std::string> > oldFormat;


}

std::string WebservConfig::getLastError() const{
	return _validator.getLastError();
}


//getter being better harder faster stronger (lol)
int WebservConfig::getPort() const {
	std::string listen = getDirective("listen");
	if (listen.empty()) return 80;

	// if form.  "host:port"
	std::vector<std::string> parts = _utils.split(listen, ':');
	if (parts.size() == 2) {
		return std::atoi(parts[1].c_str());
	}

	// if format is port onlz
	int port = std::atoi(listen.c_str());
	return (port > 0 && port <= 65535) ? port : 80;
}


std::string WebservConfig::getHost() const { // todo make bloquant error
	std::string host = getDirective("host");
	if (!host.empty()) return host;

	// Fallback: extract listen
	std::string listen = getDirective("listen");
	std::vector<std::string> parts = _utils.split(listen, ':');
	if (parts.size() == 2) {
		return parts[0];
	}

	return "127.0.0.1"; // default
}


// size_t WebservConfig::getMaxBodySize() const{
// 	std::string maxSize = getDirective("client_max_body_size");
// 	if (maxSize.empty()) return 1048576; // 1MB default

// 	size_t size = _utils.parseSize(maxSize);
// 	return (size > 0) ? size : 1048576;
// }


std::vector<std::string> WebservConfig::getAllowedMethods() const{
	std::string allowMethods = getDirective("allow_methods");

	if (allowMethods.empty()){
		std::vector<std::string> methods;
		methods.push_back("GET");
		return methods;
	}

	std::vector<std::string> parseMethods = _utils.split(allowMethods, ' ');
	std::vector<std::string> validMethods;

	for (size_t i = 0; i <parseMethods.size(); i++){
		if (_utils.isValidMethod(parseMethods[i])){
			validMethods.push_back(parseMethods[i]);
		}
	}

	//if no valid methods return get
	if (validMethods.empty()){
		validMethods.push_back("GET");
	}

	return validMethods;
}


std::string WebservConfig::getServerName() const{
	std::string srvname = getDirective("server_name");
	if (srvname.empty()) return "localhost";
	else
		return srvname;
}

std::string WebservConfig::getRoot() const{
	std::string root = getDirective("root");
	if (root.empty()) return ".";

	return ConfigParser::normalizeRootPath(root);
}

std::string WebservConfig::getIndex() const {
	std::string index = getDirective("index");
	if (index.empty())
		return "index.html";
	else
		return index;
}

std::string WebservConfig::getErrorPage(int code) const {
	std::ostringstream oss;
	oss << code;
	std::string codeStr = oss.str();

	//adding getter for multiple error code page
	std::string keyPrefixe = "error_page" + codeStr;
	std::map<std::string, std::string>::const_iterator it = _server.find(keyPrefixe);

	if (it != _server.end()){
		return it->second; //return filepath
	}
	return ""; //not found
}

size_t WebservConfig::getMaxContentLength() const{
	//for MessageParser.hpp MAX_CONTENT_LENGTH
	//return client_max_size_body or default
	ParsingUtils utils;
	std::map<std::string, std::string>::const_iterator it = _server.find("client_max_body_size");
	if (it != _server.end()){
		return utils.parseSize(it->second);
	}
	return 1048576; // like Default MEssage parser todo asking bebou for what to do
}

// bool WebservConfig::matchesServerName(const std::string& host) const{
// 	std::string serverName = getServerName();

// 	//exacte match
// 	if(host == serverName) return true;

// 	//with port
// 	std::ostringstream oss;
// 	oss << getPort();
// 	std::string hostWPort = serverName +":"+oss.str();
// 	if (host == hostWPort) return true;

// 	//matching without default port
// 	if (getPort() == 80 || getPort() == 443){
// 		size_t colonPos = host.find(':');
// 		if (colonPos != std::string::npos){
// 			std::string hostOnly = host.substr(0, colonPos);
// 			if (hostOnly == serverName) return true;
// 		}
// 	}
// 	return false;
// }

//std::vector<std::string> hostValue = _request.getHeaderValues("Host");
bool WebservConfig::isValidHostHeader(const std::string& host) const{
	if (host.empty()) return false;

	size_t colonPos = host.find(':');
	if (colonPos != std::string::npos){
		std::string portStr = host.substr(colonPos + 1);
		return _utils.isValidPort(portStr);
	}
	return true;
}


//CGI Supprot
std::string WebservConfig::getCgiPath(const std::string& location_path) const{
	std::map<std::string, std::string> loc_config = getLocationConfig(location_path, 0);
	std::map<std::string, std::string>::const_iterator it = loc_config.find("cgi_path");
	if (it != loc_config.end()){
		return it->second;
	}
	return "/usr/bin/python3";
}


std::vector<std::string> WebservConfig::getCgiExtension(const std::string& location_path) const{
	std::vector<std::string> extension;
	//todo accept only .py
	std::map<std::string, std::string> loc_config = getLocationConfig(location_path, 0);
	std::map<std::string, std::string>::const_iterator it = loc_config.find("cgi_ext");

	if (it != loc_config.end()){
		std::string cgi_ext_str = it->second;
		//if multiple extension parse them
		std::istringstream iss(cgi_ext_str);
		std::string ext;
		while (iss>>ext){
			if (ext == ".py"){
				extension.push_back(ext);
			}
		}
	}
	return extension;
}

void WebservConfig::printConfig() const {
	console::log("====Configuration===", CONF);
	console::log("config file: " + _configFile, CONF);
	console::log("", CONF);

	console::log("===Server configuration===", CONF);
	for (std::map<std::string, std::string>::const_iterator it = _server.begin(); it != _server.end(); ++it){
		console::log("   "+it->first + ": " +it->second, CONF);
	}
	console::log("", CONF);

	if (!_locations.empty()){
		console::log("==Locations==", CONF);
		for (std::map<std::string, std::map<std::string, std::string> >::const_iterator locIt = _locations.begin();
			locIt != _locations.end(); ++locIt){
				console::log("location: " + locIt->first, CONF);
				for (std::map<std::string, std::string>::const_iterator dirIt = locIt->second.begin();
					dirIt != locIt->second.end(); ++dirIt){
						console::log("    " + dirIt->first + ": "+dirIt->second, CONF);
					}
					console::log("", CONF);
			}
	}
	console::log("===========", CONF);

}


std::vector<int> WebservConfig::getAllPorts() const {
	return _listen_ports;
}
