/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_config.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jim <jim@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/01 10:35:24 by jim               #+#    #+#             */
/*   Updated: 2025/09/07 19:06:39 by jim              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser_config.hpp"
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

std::vector<std::map<std::string, std::string> > ParseConfig::parseAllServer(const std::string &configFile) const
{
	if (!validateBraces(configFile)){
		std::vector<std::map<std::string, std::string> > empty;
		return empty;
	}

	std::ifstream file(configFile.c_str());
	std::vector<std::map<std::string, std::string> > allServers;

	if (!file.is_open()){
		std::cerr << "Error : can't open file " << configFile << std::endl;
		return allServers;
	}

	std::string line;
	bool inServerBlock = false;
	std::map<std::string, std::string> currentServer;
	while(std::getline(file, line)){
		std::string okLine = ParseConfig::trim(line);

		if (okLine.empty() || okLine[0] == '#')
			continue;

		//Block Serverstart
		if (okLine == "server{" || okLine == "server {"){
			inServerBlock = true;
			currentServer.clear(); // new srv
			continue;
		}

		//end srv blokc
		if (okLine == "}" && inServerBlock){
			if (!validateServerDirectives(currentServer)){
				std::cerr<< "Error : Invalide server configuration" << std::endl;
				allServers.clear();
				return allServers;
			}
			allServers.push_back(currentServer); //save srv
			inServerBlock = false;
			continue;
		}
		//skip locations block
		if ( okLine.find("location") == 0){
			//skip to the end
			int braceCount = 1;
			while(std::getline(file, line) && braceCount > 0){
				std::string skipLine = trim(line);
				if (skipLine.find("{") != std::string::npos)
					braceCount++;
				if (skipLine=="}")
					braceCount --;
			}
			continue;
		}
		// Parse the directives
		if (inServerBlock){
			if (!okLine.empty() && okLine[okLine.size() - 1] == ';')
			okLine.erase(okLine.size() -1);

			//split by space
			size_t spacePos = okLine.find(' ');
			if (spacePos != std::string::npos){
				std::string key = okLine.substr(0, spacePos);
				std::string value = trim(okLine.substr(spacePos +1));
				currentServer[key] = value;
			}
		}
	}


	return allServers;
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
