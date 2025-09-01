/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_config.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jim <jim@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/01 10:35:24 by jim               #+#    #+#             */
/*   Updated: 2025/09/01 12:56:10 by jim              ###   ########.fr       */
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
//		std::vector<ConfParam> parseFile(const std::string &ConfigFile) const;
std::vector<ConfParam> ParseConfig::parseFile(const std::string &ConfigFile) const {
	std::ifstream file(ConfigFile.c_str());
	std::vector<ConfParam> paramConfig;

	if (!file.is_open()){
		std::cerr << "Error can't open file config : " << ConfigFile << std::endl;
		return paramConfig;
	}

	std::string line;
	while(std::getline(file, line)){
		std::string okLine = trim(line);

		if (okLine.empty() || okLine == "{" || okLine == "}")
			continue;

		size_t pos = okLine.find('#');
		std::string param;
		std::string comment;

		if (pos != std::string::npos){
			param = trim(okLine.substr(0, pos));
			comment = trim(okLine.substr(pos+1));
		}
		else
			param = okLine;

		if (!param.empty() && param[param.size() - 1] == ';')
			param.erase(param.size() -1);

		if (!param.empty() || !comment.empty()){
			paramConfig.push_back(ConfParam(param, comment));
		}
	}
	return paramConfig;

}
//		std::vector<Location> parseLocation(const std::string &ConfigFile) const;
std::vector<Location> ParseConfig::parseLocation(const std::string &ConfigFile)const{
	std::ifstream file(ConfigFile.c_str());
	std::vector<Location> locations;

	if (!file.is_open()){
		std::cerr << "Error : cant open file "<< ConfigFile <<std::endl;
		return locations;
	}

	std::string line;
	Location *currentLoc = NULL;

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
			currentLoc = new Location(path);
			continue;
		}

		//End of a block
		if (okLine == "}"){
			if (currentLoc) {
				locations.push_back(*currentLoc);
				delete currentLoc;
				currentLoc = NULL;
			}
			continue;
		}

		//line inside block
		if (currentLoc){
			size_t pos = okLine.find('#');
			std::string param;
			std::string comment;

			if (pos != std::string::npos){
				param = trim(okLine.substr(0, pos));
				comment = trim(okLine.substr(pos+1));
			}
			else
				param = okLine;

			if (!param.empty() && param[param.size() - 1 ] == ';')
				param.erase(param.size() -1);
			if (!param.empty() || !comment.empty()){
				currentLoc->addParam(ConfParam(param, comment));
			}
		}
	}
	return locations;
}
