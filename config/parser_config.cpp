/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_config.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jim <jim@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/01 10:35:24 by jim               #+#    #+#             */
/*   Updated: 2025/09/08 18:43:15 by jim              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser_config.hpp"
#include "../console/console.hpp"
#include <iostream>
#include <cstdlib>


ParseConfig::ParseConfig(){}
ParseConfig::~ParseConfig(){}

std::string ParseConfig::trim(const std::string &string) const{
	size_t start = string.find_first_not_of(" \t");
	size_t end = string.find_last_not_of(" \t");
	if (start == std::string::npos || end == std::string::npos)
		return "";
	return string.substr(start, end - start + 1);
}

std::map<std::string, std::string> ParseConfig::parseServer(const std::string &configFile) const {
	if (!validateBraces(configFile)) {
		std::map<std::string, std::string> empty;
		return empty;
	}

	std::ifstream file(configFile.c_str());
	std::map<std::string, std::string> server;

	if (!file.is_open()) {
		std::cerr << "Error : cant pen file " << configFile << std::endl;
		return server;
	}

	std::string line;
	bool inServerBlock = false;
	bool serverFound = false;

	while (std::getline(file, line)) {
		std::string okLine = trim(line);

		if (okLine.empty() || okLine[0] == '#')
			continue;

		// block srv begining
		if (okLine == "server{" || okLine == "server {") {
			if (serverFound) {
				//if more than one srv, we break
				break;
			}
			inServerBlock = true;
			server.clear();
			continue;
		}

		// end block server
		if (okLine == "}" && inServerBlock) {
			if (!validateServerDirectives(server)) {
				std::cerr << "Error : Invalid server configuration" << std::endl;
				server.clear();
				return server;
			}
			serverFound = true;
			inServerBlock = false;
			break; // no more than one srv
		}

		// Sskip locations block
		if (okLine.find("location") == 0) {
			int braceCount = 1;
			while (std::getline(file, line) && braceCount > 0) {
				std::string skipLine = trim(line);
				if (skipLine.find("{") != std::string::npos)
					braceCount++;
				if (skipLine == "}")
					braceCount--;
			}
			continue;
		}

		// parse directive in srv block
		if (inServerBlock) {
			if (!okLine.empty() && okLine[okLine.size() - 1] == ';')
				okLine.erase(okLine.size() - 1);

			size_t spacePos = okLine.find(' ');
			if (spacePos != std::string::npos) {
				std::string key = okLine.substr(0, spacePos);
				std::string value = trim(okLine.substr(spacePos + 1));
				server[key] = value;
			}
		}
	}
	return server;
}


//		std::vector<Location> parseLocation(const std::string &ConfigFile) const;
std::map<std::string, std::map<std::string, std::string> > ParseConfig::parseLocation(const std::string &ConfigFile)const{
	std::ifstream file(ConfigFile.c_str());
	std::map<std::string, std::map<std::string, std::string> > locations;

	if (!file.is_open()){
		std::cerr << "Error : cant open file "<< ConfigFile <<std::endl;
		return locations;
	}

	std::string line;
	std::string currentloc = "";

	while(std::getline(file, line)){
		std::string okLine = trim(line);

		if (okLine.empty())
			continue;

		//Start of location block
		if (okLine.find("location")== 0){
			std::istringstream iss(okLine);
			std::string keyword;
			std::string path;

			iss >> keyword >> path; // Should be like "Location /xx {"

			//deletion of '{'
			if (path[path.length() - 1] == '{'){
				path.resize(path.length() -1);
			}

			currentloc = path;
			continue;
		}

		//End of a block
		if (okLine == "}"){
			currentloc = "";
			continue;
		}

		//line inside block
		if (!currentloc.empty()){
			if (!okLine.empty() && okLine[okLine.size() -1] == ';')
				okLine.erase(okLine.size() - 1);

			//splitting first space
			size_t spacePos = okLine.find(' ');
			if (spacePos != std::string::npos){
				std::string key = okLine.substr(0, spacePos);
				std::string value = trim(okLine.substr(spacePos + 1));
				locations[currentloc][key] = value;
			}
		}
	}
	return locations;
}

bool ParseConfig::validateBraces(const std::string &ConfigFile) const {
	std::ifstream file(ConfigFile.c_str());

	if (!file.is_open()){
		std::cerr << "Error : can't open file " << ConfigFile << std::endl;
		return false;
	}

	int braceCount = 0;
	std::string line;
	int lineNumber = 0;

	while(std::getline(file, line)){
		lineNumber++;
		std::string okLine = trim(line);

		if (okLine.empty() || okLine[0] == '#')
			continue;
		for (size_t i = 0; i <okLine.length(); i++)
		{
			char c = okLine[i];
			if (c == '{')
				braceCount++;
			if (c == '}')
			{
				braceCount--;
				if (braceCount < 0){
					std::cerr << "Error: Extra '}' at line " << lineNumber << std::endl;
					return false;
				}
			}
		}
	}
	if (braceCount != 0){
		std::cerr << "Error : "<< braceCount << " unclosed '{' in config file " << std::endl;
		return false;
	}
	return true;
}

bool ParseConfig::validatePort(const std::string &port) const{
	if (port.empty()) return false;

	for (size_t i = 0; i < port.length(); i++){
		if (!isdigit(port[i])){
			std::cerr << "Error : Invalide port format: " << port << std::endl;
			return false;
		}
	}

	int portNum = atoi(port.c_str());
	if (portNum < 1 || portNum > 65535){
		std::cerr << "Error : port out of range: "<<portNum << std::endl;
		return false;
	}
	return true;
}

bool ParseConfig::validateServerDirectives(const std::map<std::string, std::string> &server) const {
	//check all obligatory (??) directives
	if (server.find("listen") == server.end()){
		std::cerr << "Error : Missing 'listen' directive" << std::endl;
		return false;
	}

	if (server.find("host") != server.end()){
		if (!validateIP(server.at("host"))){
			return false;
		}
	}

	////TODO ADD PORT RANGE
	if (!validatePort(server.at("listen"))){
		return false;
	}

	if (server.find("root") != server.end()){
		if (!validatePath(server.at("root"))){
			return false;
		}
	}

	if (server.find("error_page") != server.end()){
		std::string errorPageLine = server.at("error_page");
		size_t spacepos = errorPageLine.find(' ');
		if (spacepos != std::string::npos){
			std::string filepath = errorPageLine.substr(spacepos + 1);
			if (!validateFile(filepath)){
				return false;
			}
		}
	}

	return true;
}



bool ParseConfig::validateIP(const std::string &ip) const {

		/*
		valide IP		172.1.2.3
		invalide IP		172.01.2.3
		invalide IP		256.1.2.3

		1) split into for segment
		2) each segement is only digit
		3) each segement only between 0 -> 255
		4) each segement do not begin with 0, except if 0 is alone ////HERE
		5) only 3 dot between segement - not a the begining and end
		*/
	if (ip.empty()) return false;

	std::istringstream iss(ip);
	std::string segment;
	int count = 0;


	while (std::getline(iss, segment, '.')) {
		if (segment.empty()) return false;

		for (size_t i = 0; i < segment.length(); i++) {
			if (!isdigit(segment[i])) {
				std::cerr << "Error: Invalid IP format: " << ip <<std::endl;
				return false;
			}
			if (segment.length() > 1 && segment[0] == '0'){
				std::cerr << "Error : leading format not allowed in ip. " << segment << std::endl;
				return false;
			}
	}

		int num = atoi(segment.c_str());
		if (num < 0 || num > 255) {
			std::cerr << "Error: IP segment out of range: " << num << std::endl;
			return false;
		}
		count++;
	}

	if (count != 4) {
		std::cerr << "Error: Invalid IP format: " << ip << std::endl;
		return false;
	}

	return true;
}

bool ParseConfig::validatePath(const std::string &path) const{
	if (path.empty()) return false;

	console::log(path, ERROR);
	struct stat info;
	if (stat(path.c_str(), &info) != 0){
		std::cerr << "Error: Directory does not exist: " << path << std::endl;
		return false;
	}
	if (!(info.st_mode & S_IFDIR)) {
		std::cerr << " Error: path is not a directory: "<< path <<std::endl;
		return false;
	}
	return true;
}

bool ParseConfig::validateFile(const std::string &filepath) const {
	if ((filepath.empty())) return false;

	struct stat info;
	if (stat(filepath.c_str(), &info) != 0){
		std::cerr << "Error: file does not exist: " <<filepath << std::endl;
		return false;
	}
	if (!(info.st_mode & S_IFREG)){
		std::cerr << "Error : Path it nos a file: " <<filepath << std::endl;
		return false;
	}

	return true;
}

/*
===============Getter Setter
*/

std::string ParseConfig::getServerDirective(const std::map<std::string, std::string> &server, const std::string &directive) const {
	std::map<std::string, std::string>::const_iterator it = server.find(directive);
	if (it != server.end()){
		return it->second;
	}
	return "";
}
