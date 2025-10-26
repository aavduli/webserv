/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParsingUtils.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jim <jim@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/22 15:29:22 by jim               #+#    #+#             */
/*   Updated: 2025/10/26 15:17:10 by jim              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ParsingUtils.hpp"
#include <sstream>
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <cerrno>
#include <climits>

#ifndef SIZE_MAX
# define SIZE_MAX ((size_t) -1)
#endif

std::string ParsingUtils::trim(const std::string& str) const {
	std::string cleaned = str;
	size_t commentPos = cleaned.find('#');
	if (commentPos != std::string::npos)
		cleaned = cleaned.substr(0, commentPos);
	size_t start = cleaned.find_first_not_of(" \t\n\r");
	if (start == std::string::npos) return "";

	size_t end = cleaned.find_last_not_of(" \t\n\r");
	return cleaned.substr(start, end - start + 1);
}

std::vector<std::string> ParsingUtils::split(const std::string& str, char delimiter) const {
	std::vector<std::string> tokens;
	std::stringstream ss(str);
	std::string token;

	while( std::getline(ss, token, delimiter)){
		tokens.push_back(trim(token));
	}

	return tokens;
}

std::vector<std::string> ParsingUtils::split(const std::string& str, const std::string& delimiter) const{
	std::vector<std::string> tokens;
	size_t start = 0;
	size_t found = 0;

	while((found = str.find(delimiter, start)) != std::string::npos){
		tokens.push_back(trim(str.substr(start, found - start)));
		start = found + delimiter.length();
	}

	tokens.push_back(trim(str.substr(start)));
	return tokens;
}

bool ParsingUtils::isValidIP(const std::string& ip)const{
	std::vector<std::string> parts = split(ip, '.');
	if (parts.size() != 4) return false;

	for (size_t i = 0; i < parts.size(); i++){
		for (size_t j = 0; j < parts[i].length(); j++){
			if (!std::isdigit(parts[i][j])) return false;
		}
		char* endptr;
		errno = 0;
		long num = std::strtol(parts[i].c_str(), &endptr, 10);
		if (errno != 0 || *endptr != '\0' || num < 0 || num > 255) return false;
		if (num < 0 || num > 255) return false;

		if (parts[i].length()> 1 && parts[i][0] == '0') return false;
	}
	return true;
}

bool ParsingUtils::isValidPort(const std::string& port)const{
	if (port.empty()) return false;

	for (size_t i = 0; i < port.length(); i++){
		if (!std::isdigit(port[i])) return false;
	}
	char *endptr;
	errno = 0;
	long num = std::strtol(port.c_str(), &endptr, 10);
	if (errno != 0 || *endptr != '\0') return false;
	return (num >= 1 && num <= 65535);
}

std::string ParsingUtils::toLowerCase(const std::string& str)const{
	std::string res = str;
	std::transform(res.begin(), res.end(), res.begin(), :: tolower);
	return res;
}

bool ParsingUtils::isValidMethod(const std::string method)const{
	std::string meth = method;
	std::transform(meth.begin(), meth.end(), meth.begin(), ::toupper);

	return(meth == "GET" || meth == "POST" ||meth == "DELETE");
}

size_t ParsingUtils::parseSize(const std::string& sizeStr) const{
	if (sizeStr.empty()) return 0;

	std::string numPart = sizeStr;
	size_t multiplier = 1;

	char lastChar = sizeStr[sizeStr.length() - 1];
	if (!std::isdigit(lastChar)) {
		numPart = sizeStr.substr(0, sizeStr.length() - 1);
		switch (lastChar) {
			case 'K': case 'k': multiplier = 1024; break;
			case 'M': case 'm': multiplier = 1024 * 1024; break;
			case 'G': case 'g': multiplier = 1024 * 1024 * 1024; break;
			default: return 0;
		}
	}

	char* endptr;
	errno = 0;
	unsigned long value = std::strtoul(numPart.c_str(), &endptr, 10);
	if (errno != 0 || *endptr != '\0') return 0;
	if (value > SIZE_MAX / multiplier) return 0;

	return value * multiplier;
}
