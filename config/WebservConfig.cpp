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

const std::map<std::string, std::string>& WebservConfig::getServer() const{
	static std::map<std::string, std::string> empty;
	if (_servers.empty()) return empty;
	return _servers[0].directives;
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

	std::vector<ServerConfig> serverConfigs = parser.parseAllServers(lines);
	if(serverConfigs.empty()){
		console::log(" no server block found in config", ERROR);
		return false;
	}

	//validate all servers and lcoations
	std::cout<<"[DEBUG] parsed" << serverConfigs.size() << " servers" << std::endl;
	for (size_t i = 0; i < serverConfigs.size(); i++){
		if (!_validator.validateServerConfig(serverConfigs[i])){
			std::ostringstream oss;
			oss << "[ERROR] Server " << i << " config error: " << _validator.getLastError();
			console::log(oss.str(), ERROR);
			return false;
		}
		for (std::map<std::string, LocationConfig>::const_iterator it = serverConfigs[i].locations.begin();
			it != serverConfigs[i].locations.end(); ++it) {
			if (!_validator.validateLocationConfig(it->second)){
				std::ostringstream oss;
				oss << "[ERROR] Server " << i << " location " << it->first << " config error: " << _validator.getLastError();
				console::log(oss.str(), ERROR);
				return false;
			}
		}
	}

	_servers = serverConfigs;
	_isValid = true;
	return _isValid;


}

std::string WebservConfig::getLastError() const{
	return _validator.getLastError();
}


//getter being better harder faster stronger (lol)
int WebservConfig::getPort() const {
	if (_servers.empty() || _servers[0].listen_ports.empty()) return 0;

	std::vector<std::string> parts = _utils.split(_servers[0].listen_ports[0], ':');

	char* endptr;
	long port;
	if (parts.size() == 2){
		port = std::strtol(parts[1].c_str(), &endptr, 10);
	}else{
		port = std::strtol(_servers[0].listen_ports[0].c_str(), &endptr, 10);
	}
	return (port > 0 && port <= 65535) ? static_cast<int>(port) : 0;

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
	if (_servers.empty()) return "";
	std::ostringstream oss;
	oss << code;
	std::string codeStr = oss.str();

	//adding getter for multiple error code page
	std::string keyPrefixe = "error_page_" + codeStr;
	std::map<std::string, std::string>::const_iterator it = _servers[0].directives.find(keyPrefixe);

	if (it != _servers[0].directives.end()){
		return it->second; //return filepath
	}
	return ""; //not found
}

size_t WebservConfig::getMaxContentLength() const{
	//for MessageParser.hpp MAX_CONTENT_LENGTH
	//return client_max_size_body or default
	if (_servers.empty()) return 1048576;
	ParsingUtils utils;
	std::map<std::string, std::string>::const_iterator it = _servers[0].directives.find("client_max_body_size");
	if (it != _servers[0].directives.end()){
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

	for (size_t i = 0; i < _servers.size(); i++) {
		std::ostringstream oss;
		oss << "===Servesr " << i << " configuration==";
		console::log(oss.str(), CONF);

		for (std::map<std::string, std::string>::const_iterator it = _servers[i].directives.begin();
			it != _servers[i].directives.end(); ++it){
			console::log("   "+it->first + ": " +it->second, CONF);
		}

		for (size_t j = 0; j < _servers[i].listen_ports.size(); j++) {
			console::log("   listen: " + _servers[i].listen_ports[j], CONF);
		}
		console::log("", CONF);


		if (!_servers[i].locations.empty()) {
			console::log("   --Locations --", CONF);
			for (std::map<std::string, LocationConfig>::const_iterator locIt = _servers[i].locations.begin();
				locIt != _servers[i].locations.end(); ++locIt) {
				console::log("      location: " + locIt->first, CONF);
				for (std::map<std::string, std::string>::const_iterator dirIt = locIt->second.directives.begin();
					dirIt != locIt->second.directives.end(); ++dirIt) {
					console::log("         " + dirIt->first + ": " + dirIt->second, CONF);
				}
			}
			console::log("", CONF);
		}
	}
	console::log("=========", CONF);
}



std::vector<int> WebservConfig::getAllPorts() const {
	std::vector<int> all_ports;

	//rundown all serverus
	for(size_t i = 0; i < _servers.size(); i++){
		for(size_t j = 0; j <_servers[i].listen_ports.size(); j++ ){
			std::string listen = _servers[i].listen_ports[j];
			std::vector<std::string> parts = _utils.split(listen, ':');

			char* endptr;
			long port;
			if(parts.size() == 2){
				port = std::strtol(parts[1].c_str(), &endptr, 10);
			}else{
				port = std::strtol(listen.c_str(), &endptr, 10);
			}

			if (port > 0 && port <= 65535){
				all_ports.push_back(static_cast<int>(port));
			}
		}
	}
	return all_ports;
}

std::vector<ServerConfig> WebservConfig::getAllServers() const {
	return _servers;
}

const ServerConfig* WebservConfig::getServerByPort(int port) const{
	for (size_t i = 0; i < _servers.size(); i++){
		for (size_t j = 0; j < _servers[i].listen_ports.size(); j++){
			std::vector<std::string> parts = _utils.split(_servers[i].listen_ports[j], ':');
			char *endptr;
			long serverPort;

			if (parts.size() == 2){
				serverPort = std::strtol(parts[1].c_str(), &endptr, 10);
			}else{
				serverPort=std::strtol(_servers[i].listen_ports[j].c_str(), &endptr, 10);
			}

			if (serverPort == port){
				return &_servers[i];
			}
		}
	}
	return NULL;
}

const ServerConfig* WebservConfig::getServerByIndex(size_t index) const{
	if (index < _servers.size()){
		return &_servers[index];
	}
	return NULL;
}

size_t WebservConfig::getServerCount() const {
	return _servers.size();
}

const std::map<std::string, LocationConfig>& WebservConfig::getLocations(size_t serverIndex) const {
	static std::map<std::string, LocationConfig> empty;
	if (serverIndex >= _servers.size()) return empty;
	return _servers[serverIndex].locations;
}

bool WebservConfig::hasLocation(const std::string& path, size_t serverIndex) const {
	if (serverIndex >= _servers.size()) return false;
	return _servers[serverIndex].locations.find(path) != _servers[serverIndex].locations.end();
}

std::map<std::string, std::string> WebservConfig::getLocationConfig(const std::string& path, size_t serverIndex) const {
	if (serverIndex >= _servers.size()) {
		std::map<std::string, std::string> empty;
		return empty;
	}

	std::map<std::string, LocationConfig>::const_iterator it = _servers[serverIndex].locations.find(path);
	if (it != _servers[serverIndex].locations.end()) {
		return it->second.directives;
	}

	std::map<std::string, std::string> empty;
	return empty;
}

bool WebservConfig::hasLocation(const std::string& path) const {
	if (_servers.empty()) return false;
	return hasLocation(path, 0);
}

std::map<std::string, std::string> WebservConfig::getLocationConfig(const std::string& path) const {
	std::map<std::string, std::string> empty;
	if (_servers.empty()) return empty;
	return getLocationConfig(path, 0);
}
