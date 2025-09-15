/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigValidator.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jim <jim@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/15 14:57:26 by jim               #+#    #+#             */
/*   Updated: 2025/09/15 17:22:26 by jim              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/stat.h>
#include "ConfigValidator.hpp"


const int ConfigValidator::MIN_PORT;
const int ConfigValidator::MAX_PORT;
const size_t ConfigValidator::MAX_DIRECTIVE_LEN;

ConfigValidator::ConfigValidator(): _lastError(""){}
ConfigValidator::~ConfigValidator() {}

void ConfigValidator::setError(const std::string& error){
	_lastError = error;
}

std::string ConfigValidator::getLastError() const{
	return _lastError;
}

void ConfigValidator::clearError(){
	_lastError = "";
}

/// validation

bool ConfigValidator::validateBraces(const std::vector<std::string>& lines){ //todo correct this func
	int braceCount = 0;
	size_t lineCounter = 0;

	for (std::vector<std::string>::const_iterator it = lines.begin(); it != lines.end(); ++it){

		lineCounter++;
		const std::string& line = *it;
		if (line.empty() || line[0] == '#') continue;

		for (size_t i = 0; i < line.length(); i++){
			char c = line[i];
			if (c == '{')
				braceCount++;
			else if (c == '}'){
				braceCount--;
				if (braceCount < 0){
					std::ostringstream oss117;
					oss117 << "Extra '}' here :" << lineCounter;
					setError(oss117.str());
					return false;
				}
			}
		}
	}

	if (braceCount != 0){
		std::ostringstream oss117;
		oss117 << braceCount << " missing '}' int config file";
		setError(oss117.str());
		return false;
	}
	return true;
}

bool ConfigValidator::validateSyntaxe(const std::vector<std::string>& lines){//todo correct this func
	if (!validateBraces(lines)) return false;

	for (std::vector<std::string>::const_iterator it = lines.begin(); it != lines.end(); ++it){
		if (it->length()>MAX_DIRECTIVE_LEN){
			setError("Line too long, max : " + tostring(MAX_DIRECTIVE_LEN) + " chars");
			return false;
		}
	}
	return true;
}

bool ConfigValidator::isValidNumber(const std::string& str) const{
	if (str.empty()) return false;

	for (size_t i = 0; i < str.length(); i++){
		if (!isdigit(str[i])) return false;
	}
	return true;
}

bool ConfigValidator::isValidPath(const std::string& path) const{
	if (path.empty()) return false;

	struct stat info;
	if (stat(path.c_str(), &info) != 0) return false;
	return (info.st_mode & S_IFDIR);
}

bool ConfigValidator::isValidFile(const std::string& filepath) const{
		if (filepath.empty()) return false;

	struct stat info;
	if (stat(filepath.c_str(), &info) != 0) return false;
	return (info.st_mode & S_IFREG);
}

void ConfigValidator::setError(const std::string& error){} // TODO

//mandatory direcovec
bool ConfigValidator::validateServerConfig(const ServerConfig& config){
	if (config.directives.find("listen") == config.directives.end()){
		setError("Missing required 'listen' directive");
		return false;
	}

	//validate eache dircetives
	for (std::map<std::string, std::string>::const_iterator it = config.directives.begin();
		it != config.directives.end(); ++it){
			const std::string& key = it->first;
			const std::string& value = it->second;

			if (key == "listen" && !validatePort(value)) return false;
			if (key == "host" && !validateHost(value)) return false;
			if (key == "root" && !validateRoot(value)) return false;
			if (key == "server_name" && !validateSrvName(value)) return false;
			if (key == "error_page" && !validateErrorParge(value)) return false;
			if (key == "client_max_body_size" && !validateMBS(value)) return false;
		}

		return true;
}

bool ConfigValidator::validatePort(const std::string& port){
	if (!isValidNumber(port)){
		setError("invalid port format" + port);
		return false;
	}

	int portNum = atoi(port.c_str());
	if (portNum < MIN_PORT || portNum > MAX_PORT){
		setError("Port of out range " + port);
		return false;
	}
	return true;
}

bool ConfigValidator::validateIP(const std::string& ip){
	if (ip.empty()){
		setError("empty adresse ip");
		return false;
	}

	std::istringstream iss(ip);
	std::string segment;
	int count = 0;

	while(std::getline(iss, segment, '.')){
		if (segment.empty()){
			setError("Invalid IP format: " + segment);
			return false;
		}

		if (!isValidNumber(segment)){
			setError("Invalid IP segemt: "+segment);
			return false;
		}

		if (segment.length() > 1 && segment[0] =='0'){
			setError("DO YOU KNOW WHAT AN IP LOOK LIKE? " + segment);
			return false;
		}

		int num = atoi(segment.c_str());
		if (num < 0 || num > 255){
			setError("Ip segment ouf ot range: " + segment);
			return false;
		}
		count ++;
	}

	if (count != 4){
		setError("Invalid IP format ");
		return false;
	}
	return true;
}

bool ConfigValidator::validateHost(const std::string& host){ //can be localhost or ip
	if (host == "localhost") return true;
	return (validateIP(host));
}

bool ConfigValidator::validateRoot(const std::string& root){
	if (!isValidPath(root)){
		setError("Invalid Root directox: " + root);
		return false;
	}

	if (!hasRPerm(root)){
		setError("Can read the directoy: " + root);
		return false;
	}
	return true;
}

bool ConfigValidator::validateSrvName(const std::string& serverName){
	if (serverName.empty()){
		setError("Empty server name");
		return false;
	}

	//check for invalid char
	for (size_t i = 0; i < serverName.length(); i++){
		char c = serverName[i];
		if (!isalnum(c) && c != '.' && c != '-' && c != '_'){
			setError("invalid char in srv name: " + std::string(1, c));
			return false;
		}
	}
	return true;
}

bool ConfigValidator::validateErrorParge(const std::string& errorPageLine){
	//expected format "404 /error404.html"
	std::istringstream iss(errorPageLine);
	std::string code;
	std::string filepath;

	if (!(iss>>code>>filepath)){
		setError("Invalid error_page format: Expcetd : 'code filepath'");
		return false;
	}

	if (!isValidNumber(code)){
		setError("Invalide error code: " + code);
		return false;
	}

	int codeNum = atoi(code.c_str());
	if (codeNum < 400 || codeNum > 599){ // Todo check error code min max, and if we're gonne use them all
		setError("Error code out of range (MIN -> MAX to define): " + code);
		return false;
	}

	if (!isValidFile(filepath)){
		setError("Error page not found: "+ filepath);
		return false;
	}

	return true;
}

bool ConfigValidator::validateLocationSConfig(const LocationConfig& config){//plurals
	for (std::map<std::string, std::string>::const_iterator it = config.locations.begin(); //todo add locationS to config.
		it != config.locations.end(); ++it){
			if (!validateLocationConfig(it->second)) return false;
		}
		return true;
}

bool ConfigValidator::validateLocationConfig(const LocationConfig& config){
	if (!validateLocationPath(config.path)) return false;

	//each directive
	for (std::map<std::string, std::string>::const_iterator it = config.directives.begin();
		it != config.directives.end(); ++it){
			const std::string& key = it->first;
			const std::string& value = it->second;

			if (key == "methods" && !validateHTTPMethods(value)) return false;
			if (key == "return" && !validateRedirection(value)) return false;
			if (key == "index" && !validateIndex(value)) return false;
			if (key == "autoindex" && !validateDirectoryList(value)) return false;
			if (key == "cgi_path" && !validateCGIPath(value)) return false;
		}
		return true;
}

bool ConfigValidator::validateLocationPath(const std::string& path){
	if (path.empty()){
		setError("Empty location path");
		return false;
	}

	if (path[0] != '/'){
		setError("Location path must start with /: "+ path);
		return false;
	}
	return true;
}

bool ConfigValidator::validateHTTPMethods(const std::string& methods){
	if (methods.empty()){
		setError("should we accept empty method"+ methods ); //todo maybe accept some empty methods
		return false; //true ?
	}
	std::istringstream iss(methods);
	std::string m; // m = method

	while(iss>>m){
		if (m != "GET" && m != "POST" && m != "DELETE" ){ // todo can add more if bonuses
			setError("Invalid method: "+ m);
			return false;
		}
	}
	return true;
}

bool ConfigValidator::validateMBS(const std::string& size){
	if (!isValidNumber(size)){
		setError("Invalid client max body size format: " +size); //todo ask bebou for MBS
		return false;
	}

	long sizeNum = atol(size.c_str());
	if (sizeNum <= 0 || sizeNum > 1000000000){ //todo decrease bc 1 gb is kinda overkill see commetn above
		setError("Client MBS out of range: " + size);
		return false;
	}
	return true;
}

bool ConfigValidator::validateDirectoryList(const std::string& autoindex){
	return (autoindex == "on" || autoindex == "off");
}

bool ConfigValidator::validateCGIPath(const std::string& cgiPath){
	return (isValidPath(cgiPath));
}

bool ConfigValidator::hasRPerm(const std::string& path) const{
	return access(path.c_str(), R_OK) == 0;
}

bool ConfigValidator::hasWPerm(const std::string& path) const{
		return access(path.c_str(), W_OK) == 0;
}

bool ConfigValidator::validateTimeout(const std::string& timeout){} //todo


bool ConfigValidator::validateRedirection(const std::string& redir){} //todo

bool ConfigValidator::validateIndex(const std::string& index){} // todo


bool ConfigValidator::validatePath(const std::string& path){} //todo

bool ConfigValidator::isPortUsed(int port) const{} // todo need a free port ?
