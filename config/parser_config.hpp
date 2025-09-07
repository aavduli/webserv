/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_config.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jim <jim@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/01 10:35:44 by jim               #+#    #+#             */
/*   Updated: 2025/09/07 18:35:44 by jim              ###   ########.fr       */
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
		bool validateBraces(const std::string &configFile) const;
		bool validateServerDirectives(const std::map<std::string, std::string> &server) const;
		bool validatePort(const std::string &port) const;
		bool validateIP(const std::string &ip) const;
		bool checkDuplicates(const std::map<std::string, std::string> &directives) const;

	public :
		ParseConfig();
		~ParseConfig();

		// Global map : location["/path"]["option"] = "value"
		std::map<std::string, std::map<std::string, std::string> > parseLocation(const std::string &configFile) const;



		//vecotr to multiple server
		std::vector<std::map<std::string, std::string> > parseAllServer(const std::string &configFile) const;

};

//

#endif
