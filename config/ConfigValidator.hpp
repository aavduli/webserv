/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigValidator.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jim <jim@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/15 14:45:23 by jim               #+#    #+#             */
/*   Updated: 2025/11/04 16:28:14 by jim              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGVALIDATOR_HPP
# define CONFIGVALIDATOR_HPP
# include "ConfigData.hpp"

class ConfigValidator{
	private :
		std::string _lastError;

		static const int MIN_PORT = 1;
		static const int MAX_PORT = 65535;
		static const size_t MAX_DIRECTIVE_LEN = 1024; 

		bool isValidNumber(const std::string& str) const;
		bool isValidPath(const std::string& path) const;
		bool isValidFile(const std::string& filepath) const;
		void setError(const std::string& error);
		std::string toString(size_t num) const;

	public:
		ConfigValidator();
		~ConfigValidator();

		bool validateBraces(const std::vector<std::string>& lines);
		bool validateSyntax(const std::vector<std::string>& lines);

		bool validateNoDuplicatePorts(const std::vector<ServerConfig>& servers);


		//serveur
		bool validateServerConfig(const ServerConfig& config);
		bool validateSrvName(const std::string& serverName);
		bool validateRoot(const std::string& root);
		bool validateErrorParge(const std::string& errorPageLine);

		//location
		bool validateLocationSConfig(const LocationsConfig& config);
		bool validateLocationConfig(const LocationConfig& config);
		bool validateLocationPath(const std::string& path);
		bool validateHTTPMethods(const std::string& methods);
		bool validateRedirection(const std::string& redir);
		bool validateIndex(const std::string& index);

		bool validateMBS(const std::string& size);
		bool validateDirectoryList(const std::string& autoindex);

		bool validateCGIPath(const std::string& cgiPath);
		bool validateCGIExtension(const std::string& extension);

		bool isPortUsed(int port) const;
		bool hasRPerm(const std::string& path) const;
		bool hasWPerm(const std::string& path) const;

		std::string getLastError() const;
		void clearError();
};

#endif
