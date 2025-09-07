/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_config.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jim <jim@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/01 10:35:24 by jim               #+#    #+#             */
/*   Updated: 2025/09/07 15:24:34 by jim              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser_config.hpp"


ParseConfig::ParseConfig(){}
ParseConfig::~ParseConfig(){}

std::string ParseConfig::trim(const std::string &string) const{
	size_t start = string.find_first_not_of(" \t");
	size_t end = string.find_last_not_of(" \t");
	if (start == std::string::npos || end == std::string::npos)
		return "";
	return string.substr(start, end - start + 1);
}

std::map<std::string, std::string> ParseConfig::ParseServer(const std::string &ConfigFile) const{
	std::ifstream file(ConfigFile.c_str());
	std::map<std::string, std::string> ServerConfig;

	if (!file.is_open()){
		std::cerr << "Error: can't open file "<< ConfigFile << std::endl;
		return ServerConfig;
	}

	std::string line;
	bool inServerBlock = false;

	while(std::getline(file, line)){
		std::string okLine = trim(line);

		if (okLine.empty() || okLine[0] == '#')
			continue;

		//Block server start
		if (okLine.find("server") == 0 && okLine.find("{") != std::string::npos){
			inServerBlock = true;
			continue;
		}

		//end server block
		if (okLine == "}" && inServerBlock){
			break;
		}

		//skip locations block
		if (okLine.find("location") == 0){
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
				ServerConfig[key] = value;
			}
		}
	}
	return ServerConfig;
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
			if (path.back() == '{'){
				path.pop_back();
				path=trim(path);
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
