/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigLocation.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jim <jim@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/01 11:25:40 by jim               #+#    #+#             */
/*   Updated: 2025/09/16 11:50:29 by jim              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigLocation.hpp"
#include <iostream>

Location::Location(const std::string &path) : _path(path){}
Location::~Location(){}

void Location::addParam(const std::string &key, const std::string &value){
	_params[key] = value;
}

std::string Location::getPath() const{
	return _path;
}
//const std::map<location1<key : value> >

std::string Location::getParam(const std::string &key) const {
	std::map<std::string, std::string>::const_iterator it = _params.find(key);
	if (it != _params.end())
		return it->second;
	return "";
}

const std::map<std::string, std::string> &Location::getAllParams() const{
	return _params;
}

void Location::print() const {
	std::cout << "Location " << _path << std::endl;
	std::map<std::string, std::string>::const_iterator it ;
	for (it = _params.begin(); it != _params.end(); it++){
		std::cout << it->first << " = " << it->second << std::endl;
	}
}
