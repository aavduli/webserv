/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mainconf.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jim <jim@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/01 10:57:57 by jim               #+#    #+#             */
/*   Updated: 2025/09/01 12:58:21 by jim              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config.hpp"
#include "parser_config.hpp"

int main(){
	ParseConfig parser;
	std::vector<ConfParam> params = parser.parseFile("def.conf");
	std::vector<Location> locations = parser.parseLocation("def.conf");

	// for (size_t i = 0; i < params.size(); i++){
	// 	params[i].print();
	// }

	std::cout << "===========Server Config ==========" <<std::endl;
	for (size_t i = 0; i <params.size(); i++){
		params[i].print();
	}

	std::cout << "\n===========Location ==========" <<std::endl;
	for (size_t i = 0; i<locations.size(); i++){
		std::cout << locations[i].getPath() <<std::endl;
	}



	return 0;
}
