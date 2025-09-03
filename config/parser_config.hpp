/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_config.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jim <jim@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/01 10:35:44 by jim               #+#    #+#             */
/*   Updated: 2025/09/03 18:27:11 by jim              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_CONFIG_HPP
# define PARSER_CONFIG_HPP

#include <string>
#include <vector>
#include "config.hpp"
#include "location.hpp"

class ParseConfig{
	private:
		std::string trim(const std::string &string) const;

	public :
		ParseConfig();
		~ParseConfig();

		// Global map : location["/path"]["option"] = "value"
		std::map<std::string, std::map<std::string, std::string> > parseLocation(const std::string &configFile) const;

		//Map param server
		std::map<std::string, std::string> ParseServer(const std::string &configFile) const;
};

//

#endif
