/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebservConfig.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jim <jim@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/08 16:24:01 by jim               #+#    #+#             */
/*   Updated: 2025/10/13 16:19:21 by jim              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERVCONFIG_HPP
# define WEBSERVCONFIG_HPP

# include <string>
# include <vector>
# include <map>
# include <iostream>
# include "ConfigParser.hpp"
# include "FileReader.hpp"
# include "ConfigData.hpp"
# include "ConfigValidator.hpp"
# include "ParsingUtils.hpp"

class WebservConfig{
	private:
		ConfigParser  _parser;
		std::map<std::string, std::string> _server;
		std::map<std::string, std::map<std::string, std::string> > _locations;
		std::string		_configFile;
		bool			_isValid;
		ConfigValidator	_validator;
		ParsingUtils 	_utils;

	public:
		WebservConfig();
		WebservConfig(const std::string& configFile);
		~WebservConfig();

		std::string getLastError() const;

		bool loadConfig(const std::string& configFile);
		bool isValid() const;
		std::map<std::string, std::map<std::string, std::string> > convertToOldFormat(const LocationsConfig& locationsConfig) const;

		//validation host
		bool matchesServerName(const std::string& host) const;
		bool isValidHostHeader(const std::string& host) const;

		//srv access
		const std::map<std::string, std::string> &getServer() const;
		std::string getDirective(const std::string &directive) const;

		//better getter
		int getPort() const;
		std::string getHost() const;
		size_t getMaxBodySize() const;
		std::vector<std::string> getAllowedMethods() const;
		std::string getServerName() const;
		std::string getRoot() const;
		std::string getIndex() const;

		std::string getErrorPage(int code) const;
		size_t getMaxContentLength() const;

		//location
		bool hasLocation(const std::string& path) const;

		//locations access
		const std::map<std::string, std::map<std::string, std::string> > &getAllLocations() const;
		std::map<std::string, std::string> getLocationConfig(const std::string &path) const;

		//CGI support
		std::string getCgiPath(const std::string& location_path) const;
		std::vector<std::string> getCgiExtension(const std::string& location_path) const;

		//utils method
		void printConfig() const;
		std::string getConfigFile()const;

};

#endif
