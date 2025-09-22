/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParsingUtils.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jim <jim@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/22 15:29:22 by jim               #+#    #+#             */
/*   Updated: 2025/09/22 15:53:54 by jim              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ParsingUtils.hpp"
#include <sstream>
#include <algorithm>
#include <cctype>
#include <cstdlib>

static std::string trim(const std::string& str){
	size_t start = str.find_first_not_of("\t\n\r");
	if (start == std::string::npos) return "";

	size_t end = str.find_last_not_of("\r\t\n");
	return str.substr(start, end - start + 1);
}

static std::vector<std::string> split(const std::string& str, char delimiter){
	std::vector<std::string> tokens;
	std::stringstream ss(str);
	std::string token;

	while( std::getline(ss, token, delimiter)){
		tokens.push_back(trim(token));
	}

	return tokens;
}

static std::vector<std::string> split(const std::string& str, const std::string& delimiter){
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

static bool isValidIP(const std::string& ip){
	std::vector<std::string> parts = split(ip, '.');
	if (parts.size() != 4) return false;

	for (size_t i = 0; i < parts.size(); i++){
		int num = std::atoi(parts[i].c_str());
		if (num < 0 || num > 255) return false;

		if (parts[i].length()> 1 && parts[i][0] == '0') return false;
	}
	return true;
}

static bool isValidPort(const std::string& port){
	if (port.empty()) return false;

	int num = std::atoi(port.c_str());
	return (num >= 1 && num <= 65535);
}

static std::string toLowerCase(const std::string& str){
	std::string res = str;
	std::transform(res.begin(), res.end(), res.begin(), :: tolower);
	return res;
}

static bool isValidMethod(const std::string method){
	std::string meth = method;
	std::transform(meth.begin(), meth.end(), meth.begin(), ::toupper);

	return(meth == "GET" || meth == "POST" ||meth == "DELETE");
}
