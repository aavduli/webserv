/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv_config.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jim <jim@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/08 16:24:01 by jim               #+#    #+#             */
/*   Updated: 2025/09/15 12:00:47 by jim              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_CONFIG_HPP
# define WEBSERV_CONFIG_HPP

# include <string>
# include <vector>
# include <map>
# include <iostream>
# include "parser_config.hpp"
# include "location.hpp"
# include "file_reader.hpp"
# include "config_data.hpp"

class WebservConfig{
	private:
		ConfigParser  _parser;
		std::map<std::string, std::string> _server;
		std::map<std::string, std::map<std::string, std::string> > _locations;
		std::string		_configFile;
		bool			_isValid;

	public:
		WebservConfig();
		WebservConfig(const std::string& configFile);
		~WebservConfig();

		bool loadConfig(const std::string& configFile);
		bool isvalid() const;
		std::map<std::string, std::map<std::string, std::string> > convertToOldFormat(const LocationsConfig& locationsConfig) const;

		//srv access
		const std::map<std::string, std::string> &getServer() const;
		std::string getDirective(const std::string &directive) const;



		//locations access
		const std::map<std::string, std::map<std::string, std::string> > &getAllLocations() const;
		std::map<std::string, std::string> getLocationConfig(const std::string &path) const;

		//utils method
		void printConfig() const;
		std::string getConfigFile()const;

};

#endif
