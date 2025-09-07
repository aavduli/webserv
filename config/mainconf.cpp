/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mainconf.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jim <jim@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/01 10:57:57 by jim               #+#    #+#             */
/*   Updated: 2025/09/07 18:22:53 by jim              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config.hpp"
#include "parser_config.hpp"

/*
		std::map<std::string, std::map<std::string, std::string> > parseLocation(const std::string &configFile) const;
		std::map<std::string, std::string> ParseServer(const std::string &configFile) const;

		const std::map<std::string, std::string> &getAllParams() const;
		std::map<std::string, std::string> _params; //key = option value = valeur

		const std::map<location1<key : value> >

*/

int main(int ac, char **av){
	ParseConfig parser;

	(void) ac;
	if (ac != 2){
		std::cout << "pls enter filename  "<< std::endl;
		return 0;
	}
	std::string filename = av[1];
	filename = filename+".conf";


	std::vector<std::map<std::string, std::string> >serverConfig = parser.parseAllServer(filename);
	std::map<std::string, std::map<std::string, std::string> > locations = parser.parseLocation(filename);
	std::map<std::string, std::string>::const_iterator it;
	std::map<std::string, std::map<std::string, std::string> >::const_iterator itt;

	std::cout << "===========Server Config ==========" <<std::endl;

	//std::cout << serverConfig.size() <<std::endl;

	for (int i = 0; i  < (int)serverConfig.size(); i++){
		std::map<std::string, std::string>::iterator it;
		for (it = serverConfig[i].begin(); it != serverConfig[i].end(); it++){
			std::cout << it->first << " : " << it->second << std::endl;
		}
	}

	std::cout << "\n===========Location ==========" <<std::endl;
	for (itt = locations.begin(); itt != locations.end(); itt++){
		std::cout << "location: " << itt->first << std::endl;
		for (it = itt->second.begin(); it != itt->second.end() ; it++){
			std::cout << "Param: " << it->first << " = " <<it->second << std::endl;
		}
	}




	return 0;
}
