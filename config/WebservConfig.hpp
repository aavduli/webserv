/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebservConfig.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jim <jim@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/08 16:24:01 by jim               #+#    #+#             */
/*   Updated: 2025/09/16 13:38:40 by jim              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERVCONFIG_HPP
# define WEBSERVCONFIG_HPP

# include <string>
# include <vector>
# include <map>
# include <iostream>
# include "ConfigParser.hpp"
# include "ConfigLocation.hpp"
# include "FileReader.hpp"
# include "config_data.hpp"
# include "ConfigValidator.hpp"

class WebservConfig{
	private:
		ConfigParser  _parser;
		std::map<std::string, std::string> _server;
		std::map<std::string, std::map<std::string, std::string> > _locations;
		std::string		_configFile;
		bool			_isValid;
		ConfigValidator	_validator;

	public:
		WebservConfig();
		WebservConfig(const std::string& configFile);
		~WebservConfig();

		std::string getLastError() const;

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
