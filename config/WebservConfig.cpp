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
#include <cstdlib>
#include <sstream>
#include "ParsingUtils.hpp"

WebservConfig::WebservConfig(): _isValid(false){}

WebservConfig::WebservConfig(const std::string& configFile):
	_configFile(configFile), _isValid(false)
{
	loadConfig(configFile);
}

WebservConfig::~WebservConfig(){}

bool WebservConfig::isvalid() const{
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

std::map<std::string, std::string> WebservConfig::getLocationConfig(const std::string& path) const{
	std::map<std::string, std::map<std::string, std::string> >::const_iterator it = _locations.find(path);
	if (it != _locations.end())
		return it->second;
	std::map<std::string, std::string> empty;
	return empty;
}

void WebservConfig::printConfig() const {
	std::cout << "=== SERVER CONFIG ===" << std::endl;

	for (std::map<std::string, std::string>::const_iterator it = _server.begin();
		it != _server.end(); ++it) {
		std::cout << it->first << ": " << it->second << std::endl;
	}

	std::cout << "\n=== LOCATIONS ===" << std::endl;
	for (std::map<std::string, std::map<std::string, std::string> >::const_iterator it = _locations.begin();
		it != _locations.end(); ++it) {
		std::cout << "Location " << it->first << ":" << std::endl;

		for (std::map<std::string, std::string>::const_iterator param = it->second.begin();
			param != it->second.end(); ++param) {
			std::cout << "  " << param->first << " = " << param->second << std::endl;
		}
	}
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
	FileReader reader;
	ConfigParser parser;

	std::vector<std::string> lines = reader.readLines(configFile);
	if (lines.empty()) return false;

	if (!_validator.validateSyntax(lines)){
		std::cerr << "Syntax error : " << _validator.getLastError() << std::endl;
		return false;
	}

	ServerConfig serverConfig = parser.parseServer(lines);
	LocationsConfig locationsConfig = parser.parseLocations(lines);

	if (!_validator.validateServerConfig(serverConfig)){
		std::cerr << "Server config error: " <<_validator.getLastError() <<std::endl;
		return false;
	}

	for (std::map<std::string, LocationConfig>::const_iterator it = locationsConfig.locations.begin();
		it != locationsConfig.locations.end(); ++it){
			if (!_validator.validateLocationConfig(it->second)){
				std::cerr<< "Location config error: "<< _validator.getLastError() << std::endl;
				return false;
			}
		}

	_server = serverConfig.directives;
	_locations = convertToOldFormat(locationsConfig);
	_isValid = true;
	return _isValid;
}

std::map<std::string, std::map<std::string, std::string> > WebservConfig::convertToOldFormat(const LocationsConfig& locationsConfig) const {
	std::map<std::string, std::map<std::string, std::string> > oldFormat;

	for (std::map<std::string, LocationConfig>::const_iterator it = locationsConfig.locations.begin();
		it != locationsConfig.locations.end(); ++it) {
		oldFormat[it->first] = it->second.directives;
	}

	return oldFormat;
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


size_t WebservConfig::getMaxBodySize() const{
	std::string maxSize = getDirective("max_size_body");
	if (maxSize.empty()) return 1048576; // 1MB default

	size_t size = _utils.parseSize(maxSize);
	return (size > 0) ? size : 1048576;
}


std::vector<std::string> WebservConfig::getAllowedMethods() const{
	std::string allowMethods = getDirective("allow_methods");

	if (allowMethods.empty()){
		std::vector<std::string> methods;
		methods.push_back("GET");
		return methods;
	}

	return _utils.split(allowMethods, ' ');
}


std::string WebservConfig::getServerName() const{
	return getDirective("server_name");
}

std::string WebservConfig::getRoot() const{
	return getDirective("root");
}

std::string WebservConfig::getIndex() const {
	return getDirective("index");
}

std::string WebservConfig::getErrorPage(int code) const {
	std::ostringstream oss;
	oss << code;
	std::string codeStr = oss.str();

	//adding getter for multiple error code page
	std::string keyPrefixe = "error_page_" + codeStr;
	std::map<std::string, std::string>::const_iterator it = _server.find(keyPrefixe);

	if (it != _server.end()){
		return it->second; //return filepath
	}
	else{
		std::cout << "keyPrefix: "<< keyPrefixe << std::endl;
	}

	if (it != _server.end()){
		//parsing "404 /error404.html"
		std::istringstream iss(it->second);
		std::string pageCode;
		std::string filepath;

		if (iss >> pageCode >> filepath && pageCode == codeStr){
			return filepath;
		}
	}
	return ""; //not found
}

size_t WebservConfig::getMaxContentLength() const{
	//for MessageParser.hpp MAX_CONTENT_LENGTH
	//return max_size_body or default
	ParsingUtils utils;
	std::map<std::string, std::string>::const_iterator it = _server.find("client_max_body_size");
	if (it != _server.end()){
		return utils.parseSize(it->second);
	}
	return 1000000; // like Default MEssage parser todo asking bebou for what to do
}

bool WebservConfig::hasLocation(const std::string& path) const{
	return _locations.find(path) != _locations.end();
}
