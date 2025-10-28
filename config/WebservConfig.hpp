/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebservConfig.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jim <jim@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/08 16:24:01 by jim               #+#    #+#             */
/*   Updated: 2025/10/28 13:10:43 by jim              ###   ########.fr       */
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
		std::vector<ServerConfig> _servers;
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

		std::vector<ServerConfig> getAllServers() const;
		const ServerConfig* getServerByPort(int port) const;
		const ServerConfig* getServerByIndex(size_t index) const;
		size_t getServerCount() const;

		std::vector<int> getAllPorts() const;

		std::string getErrorPage(int code) const;
		size_t getMaxContentLength() const;

		// default server
		bool hasLocation(const std::string& path) const;
		std::map<std::string, std::string> getLocationConfig(const std::string &path) const;

		//location
		bool hasLocation(const std::string& path, size_t serverIndex) const;
		//locations access
		const std::map<std::string, LocationConfig>& getLocations(size_t serverIndex) const;
		std::map<std::string, std::string> getLocationConfig(const std::string &path, size_t serverIndex) const;

		//CGI support
		std::string getCgiPath(const std::string& location_path) const;
		std::vector<std::string> getCgiExtension(const std::string& location_path) const;

		//utils method
		std::string getConfigFile()const;
		void printConfig() const;



};

#endif
