/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_config.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jim <jim@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/15 10:45:14 by jim               #+#    #+#             */
/*   Updated: 2025/09/15 13:58:59 by jim              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser_config.hpp"

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

			case IN_SERVER_BLOCK:{ //added { to create scope for directive var
				if (line == BLOCK_END)
					return config;
				if (line.find(LOCATION_START) == 0){
					skipBlock(i, lines);
					continue;
				}

					std::pair<std::string, std::string>  directive = parseDirective(line);
					if (!directive.first.empty()){
						config.directives[directive.first] = directive.second;
					}
				break;
			}

			case IN_LOCATION_BLOCK:
				//TODO loc
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

	return std::make_pair(key, value);

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


LocationsConfig ConfigParser::parseLocations(const std::vector<std::string> & lines) const{
	(void)lines;
	LocationsConfig config;
	//TODO : parse locations
	return config;
}

bool ConfigParser::validateBraces(const std::vector<std::string>& lines) const{
	(void) lines;
	//TODO :: change validate braces
	return true;
}
