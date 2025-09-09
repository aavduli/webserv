/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv_config.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jim <jim@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/08 16:37:53 by jim               #+#    #+#             */
/*   Updated: 2025/09/08 18:25:31 by jim              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv_config.hpp"

WebservConfig::WebservConfig(): _isValid(false){}

WebservConfig::WebservConfig(const std::string &configFile) :
	_configFile(configFile), _isValid(false)
{
	loadConfig(configFile);
}

WebservConfig::~WebservConfig() {}

bool WebservConfig::loadConfig(const std::string &configFile){
	_configFile = configFile;
	_isValid = false;

	//PArsing srv(
	_server = _parser.parseServer(configFile);
	if (_server.empty()){
		std::cerr << "Error: No valide server found in config" << std::endl;
		return false;
	}

	//locations
	_locations = _parser.parseLocation(configFile);
	WebservConfig::printConfig();

	_isValid = true;


	return true;
}

bool WebservConfig::isvalid() const {
	return _isValid;
}

const std::map<std::string, std::string> &WebservConfig::getServer() const{
	 return _server;
}

const std::map<std::string, std::map<std::string, std::string> > &WebservConfig::getAllLocations() const{
	return _locations;
}

std::map<std::string, std::string> WebservConfig::getLocationConfig(const std::string &path) const{
	std::map<std::string, std::map<std::string, std::string> >::const_iterator it = _locations.find(path);
	if (it != _locations.end()) {
		return it->second;
	}
	std::map<std::string, std::string> empty;
	return empty;
}

std::string WebservConfig::getDirective(const std::string &directive) const {
	std::map<std::string, std::string>::const_iterator it = _server.find(directive);
	return (it != _server.end()) ? it->second : "";  //"" empty if nonexistent
}

void WebservConfig::printConfig() const {
	std::cout << "=== SERVER CONFIG ===" << std::endl;
	std::cout << "Port: " << getDirective("listen") << std::endl;
	std::cout << "Host: " << getDirective("host") << std::endl;
	std::cout << "Root: " << getDirective("root") << std::endl;
	std::cout << "Server Name: " << getDirective("server_name") << std::endl;

	std::cout << "\n=== LOCATIONS ===" << std::endl;
	for (std::map<std::string, std::map<std::string, std::string>
>::const_iterator it = _locations.begin();
		it != _locations.end(); ++it) {
		std::cout << "Location " << it->first << ":" << std::endl;
		for (std::map<std::string, std::string>::const_iterator param =
it->second.begin();
			param != it->second.end(); ++param) {
			std::cout << "  " << param->first << " = " << param->second <<
std::endl;
		}
	}
}

std::string WebservConfig::getConfigFile() const {
	return _configFile;
}
