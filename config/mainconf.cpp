/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mainconf.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jim <jim@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/01 10:57:57 by jim               #+#    #+#             */
/*   Updated: 2025/09/08 17:51:59 by jim              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config.hpp"
#include "parser_config.hpp"

#include "config.hpp"
#include "webserv_config.hpp"  // Utilise ta nouvelle classe

int main(int ac, char **av) {
	if (ac != 2) {
		std::cerr << "Usage: " << av[0] << " <config_file>" << std::endl;
		return 1;
	}

	std::string configFile = av[1];
	if (configFile.find(".conf") == std::string::npos) {
		configFile += ".conf";
	}

	WebservConfig config(configFile);

	if (!config.isvalid()) {
		std::cerr << "Error: Invalid configuration file" << std::endl;
		return 1;
	}

	std::cout << "============ CONFIGURATION ============" << std::endl;
	config.printConfig();

	return 0;
}
