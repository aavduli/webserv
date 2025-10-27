/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jim <jim@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/15 10:45:14 by jim               #+#    #+#             */
/*   Updated: 2025/10/27 17:49:33 by jim              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigParser.hpp"
#include <iostream>
#include <sstream>

const std::string ConfigParser::SERVER_START = "server";
const std::string ConfigParser::LOCATION_START = "location";
const std::string ConfigParser::BLOCK_END = "}";

ServerConfig ConfigParser::parseServer(const std::vector<std::string>& lines) const{
	ServerConfig config;
	ParseState state = OUTSIDE_BLOCK;

	for (size_t i = 0 ; i <lines.size(); ++i){
		const std::string& line = lines[i];

		if (line.empty() || line[0] == '#') continue;

		switch(state){
			case OUTSIDE_BLOCK:
				if (line.find(SERVER_START) == 0)
					state = IN_SERVER_BLOCK;
				break;

			case IN_SERVER_BLOCK:{
				if (line == BLOCK_END)
					return config;
				if (line.find(LOCATION_START) == 0){
					skipBlock(i, lines);
					continue;
				}
				std::pair<std::string, std::string> directive = parseDirective(line);
				if (!directive.first.empty()){
					if (directive.first == "listen"){
						config.listen_ports.push_back(directive.second);
					}
					if (directive.first == "error_page"){
						std::istringstream iss (directive.second);
						std::string code;
						std::string filepath;
						if (iss >> code >> filepath){
							std::string key = "error_page_"+code;
							config.directives[key] = filepath;
						}
						else{
							config.directives[directive.first] = directive.second;
						}
					}else{
						config.directives[directive.first] = directive.second;
					}
				}
			}break;

			case IN_LOCATION_BLOCK: // TODO
				break;
		}
	}
	return config;
}

std::pair<std::string, std::string> ConfigParser::parseDirective(const std::string& line) const{
	std::string cleanLine = line;
	if (!cleanLine.empty() && cleanLine[cleanLine.size()-1] == ';')
		cleanLine.erase(cleanLine.size() -1);

	size_t spacePos = cleanLine.find(' ');
	if (spacePos == std::string::npos)
		return std::make_pair("","");

	std::string key = cleanLine.substr(0, spacePos);
	std::string value = cleanLine.substr(spacePos +1);

	//normalize root
	if (key == "root")
		value = normalizeRootPath(value);

	return std::make_pair(key, value);

}

std::string ConfigParser::normalizeRootPath(const std::string& path){
	if (path.empty()) return "./";

	std::string normalized = path;
	//1. add ./ if ,missing
	if (normalized.length() < 2 || normalized.substr(0,2) != "./"){
		if (normalized[0] == '/'){
			// /var/www -> ./var/www
			normalized = "." + normalized;
		}
		else{
			//www -> ./www
			normalized = "./" + normalized;
		}
	}

	//delete the / at the end if exist
	while (!normalized.empty() && normalized[normalized.length() -1] == '/'){
		normalized.erase(normalized.length() -1);
	}

	return normalized;
}

void ConfigParser::skipBlock(size_t& index, const std::vector<std::string>& lines) const{
	int braceCount = 1;
	++index;

	while(index < lines.size() && braceCount > 0){
		if (lines[index].find("{") != std::string::npos) braceCount ++;
		if (lines[index] == BLOCK_END) braceCount --;
		++index;
	}
	--index;
}


LocationsConfig ConfigParser::parseLocations(const std::vector<std::string> & lines) const{ //TODO : parse locations
	LocationsConfig config;
	std::string currentLocationPath = "";
	LocationConfig currentLocation;

	for (size_t i = 0; i < lines.size();++i){
		const std::string& line = lines[i];

		if (line.empty() || line[0] == '#') continue;

		//location block
		if (line.find(LOCATION_START) == 0){
			//save prev. location
			if (!currentLocationPath.empty())
				config.locations[currentLocationPath] = currentLocation;

			//new loc
			std::istringstream iss(line);
			std::string keyword;
			std::string path;
			iss >>keyword >> path;

			//delte the {
			if (!path.empty() && path[path.length() -1] == '{')
				path.resize(path.length() -1);

			currentLocationPath = path;
			currentLocation.path = path;
			currentLocation.directives.clear();
			continue;
		}

		//end loca block
		if (line == BLOCK_END && !currentLocationPath.empty()){
			//save current location
			config.locations[currentLocationPath] = currentLocation;
			currentLocationPath = "";
			continue;
		}

		//parse directive inside loc. block
		if (!currentLocationPath.empty()){
			std::pair<std::string, std::string> directive = parseDirective(line);
			if (!directive.first.empty())
				currentLocation.directives[directive.first] = directive.second;
		}
	}

	//save last location
	if (!currentLocationPath.empty())
		config.locations[currentLocationPath] = currentLocation;

	return config;
}

std::vector<ServerConfig> ConfigParser::parseAllServers(const std::vector<std::string>&  lines) const{
	std::vector<ServerConfig> servers;
	ServerConfig currentServer;
	ParseState state = OUTSIDE_BLOCK;
	bool inServer = false;

	for(size_t i = 0; i <lines.size(); ++i){
		const std::string& line = lines[i];

		if (line.empty() || line[0] == '#') continue;
		//begin of block server
		if (line.find(SERVER_START) == 0 && state == OUTSIDE_BLOCK){
			if (inServer)
				servers.push_back(currentServer);
			//new srv
			currentServer = ServerConfig();
			state = IN_SERVER_BLOCK;
			inServer = true;
			continue;
		}

		//in server bloc
		if (state == IN_SERVER_BLOCK){
			if (line == BLOCK_END){
				servers.push_back(currentServer);
				state = OUTSIDE_BLOCK;
				inServer = false;
				continue;
			}

			//skip loca block
			if (line.find(LOCATION_START) == 0){
				skipBlock(i, lines);
				continue;
			}

			std::pair<std::string, std::string> directive = parseDirective(line);
			if (!directive.first.empty()){
				if (directive.first == "listen"){
					currentServer.listen_ports.push_back(directive.second);
				}
				if (directive.first == "error_page"){
					std::istringstream iss(directive.second);
					std::string code;
					std::string filepath;
					if (iss >> code >> filepath){
						std::string key = "error_page_" +code;
						currentServer.directives[key] = filepath;
					}else{
						currentServer.directives[directive.first] = directive.second;
					}
				}else{
					currentServer.directives[directive.first] = directive.second;
				}
			}
		}
	}
	return servers;

}
