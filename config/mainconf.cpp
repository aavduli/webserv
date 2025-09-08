/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mainconf.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jim <jim@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/01 10:57:57 by jim               #+#    #+#             */
/*   Updated: 2025/09/08 14:32:41 by jim              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config.hpp"
#include "parser_config.hpp"

int main(int ac, char **av){
	if (ac != 2){
		std::cerr << "pls tell me the filename  " << std::endl;
		return 1;
	}

	std::string configFile = av[1];
	if (configFile.find(".conf") == std::string::npos){
		configFile +=".conf";
	}

	ParseConfig srv;

	if (!srv.validateBraces(configFile)){
		std::cerr << " Error : Invalid conf file syntaxe" << std::endl;
		return 1;
	}

	std::vector<std::map<std::string, std::string> > servers;
	std::map<std::string, std::map<std::string, std::string> > locations;

	try{
		servers = srv.parseAllServer(configFile);
		locations = srv.parseLocation(configFile);
	}catch( const std::exception &e){
		std::cerr << "PArsing failed. Reason :" <<e.what() <<std::endl;
		return 1;
	}

	if (servers.empty()){
		std::cerr << "Error: No Valid servers found in conf file" <<std::endl;
		return 1;
	}

	std::cout << "============configuration ============" << std::endl <<std::endl;
	for (size_t i = 0 ; i < servers.size(); i++){
		std::cout << "\n ===== Server : "<< srv.getServerDirective(servers[i], "server_name") << std::endl;
		std::cout << "\n ===== Host : "<< srv.getServerDirective(servers[i], "host") << std::endl;
		std::cout << "\n ===== root : "<< srv.getServerDirective(servers[i], "root") << std::endl;
		std::cout << "\n ===== index : "<< srv.getServerDirective(servers[i], "index") << std::endl;

		if (!locations.empty()){
			std::cout << "\n    Locations:"<<std::endl;
			std::map<std::string, std::map<std::string, std::string> >::const_iterator itt;
			std::map<std::string, std::string>::const_iterator it;
			for (itt = locations.begin(); itt != locations.end(); itt++){
				std::cout << "    ├── Path: " << itt->first << std::endl;{
					for (it = itt->second.begin(); it != itt->second.end(); it++){
						std::cout << "    │   ├── " << it->first << ": " << it->second << std::endl;
					}
				}
			}
		}

	}

	return 0;
}
