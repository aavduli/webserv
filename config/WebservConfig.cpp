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

	// Parcourir TOUTES les directives dynamiquement
	for (std::map<std::string, std::string>::const_iterator it = _server.begin();
		it != _server.end(); ++it) {
		std::cout << it->first << ": " << it->second << std::endl;
	}

	std::cout << "\n=== LOCATIONS ===" << std::endl;
	for (std::map<std::string, std::map<std::string, std::string> >::const_iterator it = _locations.begin();
		it != _locations.end(); ++it) {
		std::cout << "Location " << it->first << ":" << std::endl;

		// Parcourir toutes les directives de cette location
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
