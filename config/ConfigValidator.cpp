/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigValidator.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jim <jim@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/15 14:57:26 by jim               #+#    #+#             */
/*   Updated: 2025/09/22 16:52:52 by jim              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/stat.h>
#include "ConfigValidator.hpp"
#include "../console/console.hpp"
#include "ParsingUtils.hpp"

const int ConfigValidator::MIN_PORT;
const int ConfigValidator::MAX_PORT;
const size_t ConfigValidator::MAX_DIRECTIVE_LEN;

const int BLOCKINGERROR = 0; //0 non blocking validator error 1 blocking validator error

ConfigValidator::ConfigValidator(): _lastError(""){}
ConfigValidator::~ConfigValidator() {}

void ConfigValidator::setError(const std::string& error){
	_lastError = error;
}

std::string ConfigValidator::getLastError() const{//todo oo correct this func
	return _lastError;
}

void ConfigValidator::clearError(){
	_lastError = "";
}

/// validation

bool ConfigValidator::validateBraces(const std::vector<std::string>& lines){
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
					console::log(_lastError, CONF);
					return (BLOCKINGERROR ? false : true );
				}
			}
		}
	}

	if (braceCount != 0){
		std::ostringstream oss117;
		oss117 << braceCount << " missing '}' int config file";
		setError(oss117.str());
		console::log(_lastError, CONF);
		return (BLOCKINGERROR ? false : true );
	}
	return true;
}

bool ConfigValidator::validateSyntax(const std::vector<std::string>& lines) {
	if (!validateBraces(lines)) return false;

	for (std::vector<std::string>::const_iterator it = lines.begin();
		it != lines.end(); ++it) {
		if (it->length() > MAX_DIRECTIVE_LEN) {
			setError("Line too long, max: " + toString(MAX_DIRECTIVE_LEN) + " chars");
			console::log(_lastError, CONF);
			return (BLOCKINGERROR ? false : true );
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

std::string ConfigValidator::toString(size_t num) const {
	std::ostringstream oss;
	oss << num;
	return oss.str();
}

//mandatory direcovec
bool ConfigValidator::validateServerConfig(const ServerConfig& config) {
	ParsingUtils utils;

	// check mandatory dircrive
	if (config.directives.find("listen") == config.directives.end()) {
		setError("Missing required 'listen' directive");
		console::log(_lastError, CONF);
		return (BLOCKINGERROR ? false : true);
	}

	// validate each directive
	for (std::map<std::string, std::string>::const_iterator it = config.directives.begin();
		it != config.directives.end(); ++it) {

		const std::string& key = it->first;
		const std::string& value = it->second;

		if (key == "listen") {
			std::vector<std::string> parts = utils.split(value, ':');
			std::string portStr = (parts.size() == 2) ? parts[1] : value;
			if (!utils.isValidPort(portStr)) {
				_lastError = "Invalid port: " + portStr;
				return false;
			}
		}
		else if (key == "host") {
			if (!utils.isValidIP(value)) {
				_lastError = "Invalid IP address: " + value;
				return false;
			}
		}
		else if (key == "allow_methods") {
			std::vector<std::string> methods = utils.split(value, ' ');
			for (size_t i = 0; i < methods.size(); i++) {
				if (!utils.isValidMethod(methods[i])) {
					_lastError = "Invalid HTTP method: " + methods[i];
					return false;
				}
			}
		}
		else if (key == "root" && !validateRoot(value)) {
			return false;
		}
		else if (key == "server_name" && !validateSrvName(value)) {
			return false;
		}
		else if (key == "error_page" && !validateErrorParge(value)) {
			return false;
		}
		else if (key == "client_max_body_size" && !validateMBS(value)) {
			return false;
		}
	}

	return true;
}


bool ConfigValidator::validatePort(const std::string& port){
	if (!isValidNumber(port)){
		setError("invalid port format" + port);
		console::log(_lastError, CONF);
		return (BLOCKINGERROR ? false : true );
	}

	int portNum = atoi(port.c_str());
	if (portNum < MIN_PORT || portNum > MAX_PORT){
		setError("Port of out range " + port);
		console::log(_lastError, CONF);
		return (BLOCKINGERROR ? false : true );
	}
	return true;
}

bool ConfigValidator::validateIP(const std::string& ip){
	if (ip.empty()){
		setError("empty adresse ip");
		console::log(_lastError, CONF);
		return (BLOCKINGERROR ? false : true );
	}

	std::istringstream iss(ip);
	std::string segment;
	int count = 0;

	while(std::getline(iss, segment, '.')){
		if (segment.empty()){
			setError("Invalid IP format: " + segment);
			console::log(_lastError, CONF);
			return (BLOCKINGERROR ? false : true );
		}

		if (!isValidNumber(segment)){
			setError("Invalid IP segemt: "+segment);
			console::log(_lastError, CONF);
			return (BLOCKINGERROR ? false : true );
		}

		if (segment.length() > 1 && segment[0] =='0'){
			setError("DO YOU KNOW WHAT AN IP LOOK LIKE? " + segment);
			console::log(_lastError, CONF);
			return (BLOCKINGERROR ? false : true );
		}

		int num = atoi(segment.c_str());
		if (num < 0 || num > 255){
			setError("Ip segment ouf ot range: " + segment);
			console::log(_lastError, CONF);
			return (BLOCKINGERROR ? false : true );
		}
		count ++;
	}

	if (count != 4){
		setError("Invalid IP format ");
		console::log(_lastError, CONF);
		return (BLOCKINGERROR ? false : true );
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
		console::log(_lastError, CONF);
		return (BLOCKINGERROR ? false : true );
	}

	if (!hasRPerm(root)){
		setError("Can read the directoy: " + root);
		console::log(_lastError, CONF);
		return (BLOCKINGERROR ? false : true );
	}
	return true;
}

bool ConfigValidator::validateSrvName(const std::string& serverName){
	if (serverName.empty()){
		setError("Empty server name");
		return (BLOCKINGERROR ? false : true );
	}

	//check for invalid char
	for (size_t i = 0; i < serverName.length(); i++){
		char c = serverName[i];
		if (!isalnum(c) && c != '.' && c != '-' && c != '_'){
			setError("invalid char in srv name: " + std::string(1, c));
			console::log(_lastError, CONF);
			return (BLOCKINGERROR ? false : true );
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
		console::log(_lastError, CONF);
		return (BLOCKINGERROR ? false : true );
	}

	if (!isValidNumber(code)){
		setError("Invalide error code: " + code);
		console::log(_lastError, CONF);
		return (BLOCKINGERROR ? false : true );
	}

	int codeNum = atoi(code.c_str());
	if (codeNum < 400 || codeNum > 599){ // Todo check error code min max, and if we're gonne use them all
		setError("Error code out of range (MIN -> MAX to define): " + code);
		console::log(_lastError, CONF);
		return (BLOCKINGERROR ? false : true );
	}

	if (!isValidFile(filepath)){
		setError("Error page not found: "+ filepath);
		console::log(_lastError, CONF);
		return (BLOCKINGERROR ? false : true );
	}

	return true;
}

bool ConfigValidator::validateLocationSConfig(const LocationsConfig& config) {
	for (std::map<std::string, LocationConfig>::const_iterator it =
config.locations.begin();
		it != config.locations.end(); ++it) {
		if (!validateLocationConfig(it->second)) return false;  // call the singular version
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
		console::log(_lastError, CONF);
		return (BLOCKINGERROR ? false : true );
	}

	if (path[0] != '/'){
		setError("Location path must start with /: "+ path);
		console::log(_lastError, CONF);
		return (BLOCKINGERROR ? false : true );
	}
	return true;
}

bool ConfigValidator::validateHTTPMethods(const std::string& methods){
	if (methods.empty()){
		setError("should we accept empty method"+ methods ); //todo maybe accept some empty methods
		return (BLOCKINGERROR ? false : true ); //true ?
	}
	std::istringstream iss(methods);
	std::string m; // m = method

	while(iss>>m){
		if (m != "GET" && m != "POST" && m != "DELETE" ){ // todo can add more if bonuses
			setError("Invalid method: "+ m);
			console::log(_lastError, CONF);
			return (BLOCKINGERROR ? false : true );
		}
	}
	return true;
}

bool ConfigValidator::validateMBS(const std::string& size){
	if (!isValidNumber(size)){
		setError("Invalid client max body size format: " +size); //todo ask bebou for MBS
		console::log(_lastError, CONF);
		return (BLOCKINGERROR ? false : true );
	}

	long sizeNum = atol(size.c_str());
	if (sizeNum <= 0 || sizeNum > 1000000000){ //todo decrease bc 1 gb is kinda overkill see commetn above
		setError("Client MBS out of range: " + size);
		console::log(_lastError, CONF);
		return (BLOCKINGERROR ? false : true );
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

bool ConfigValidator::validateTimeout(const std::string& timeout){ //todo
	(void) timeout;
	return true;
}


bool ConfigValidator::validateRedirection(const std::string& redir){ //todo
	(void) redir;
	return true;
}

bool ConfigValidator::validateIndex(const std::string& index){ //todo
	(void) index;
	return true;
}


bool ConfigValidator::validatePath(const std::string& path){ //todo
	(void) path;
	return true;
}

bool ConfigValidator::isPortUsed(int port) const{ //todo do we need a check for free port?
	(void) port;
	return false; // /!\ watch out invers, false = free port
}
