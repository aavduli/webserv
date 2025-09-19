/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_config.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jim <jim@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/01 10:35:44 by jim               #+#    #+#             */
/*   Updated: 2025/09/08 17:27:34 by jim              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_CONFIG_HPP
# define PARSER_CONFIG_HPP

#include <string>
#include <vector>
#include <map>
#include <sys/stat.h>
#include "config.hpp"
#include "location.hpp"

class ParseConfig{
	private:
		std::string trim(const std::string &string) const;
		bool validateServerDirectives(const std::map<std::string, std::string> &server) const;
		bool validatePort(const std::string &port) const;
		bool validateIP(const std::string &ip) const;
		bool checkDuplicates(const std::map<std::string, std::string> &directives) const;
		bool validatePath(const std::string &path) const;
		bool validateFile(const std::string &path) const;


		public :
		bool validateBraces(const std::string &configFile) const;
		ParseConfig();
		~ParseConfig();

		// Global map : location["/path"]["option"] = "value"
		std::map<std::string, std::map<std::string, std::string> > parseLocation(const std::string &configFile) const;
		//vecotr to multiple server
		//std::vector<std::map<std::string, std::string> > parseAllServer(const std::string &configFile) const;
		std::map<std::string, std::string> parseServer(const std::string &configFile) const;

		//get-setradio
		//const std::vector<std::map<std::string, std::string> > &getAllServers(const std::string &configFile);
		const std::map<std::string, std::map<std::string, std::string> > &getAllLocations(const std::string &configFile);

		//get server directive
		std::string getServerDirective(const std::map<std::string, std::string> &server, const std::string &directive) const;
		int getServerPort(const std::map<std::string, std::string> &server) const;
		std::string getServerName(const std::map<std::string, std::string> &server) const;
		std::string getServerHost(const std::map<std::string, std::string> &server) const;
		std::string getServerRoot(const std::map<std::string, std::string> &server) const;

		//get lcoations
		std::vector<std::string> getLocationPath(const std::string &configfile);
		std::map<std::string, std::string> getLocationDirectives(const std::string &location) const;
};

//

#endif
