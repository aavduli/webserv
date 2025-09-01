/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jim <jim@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/01 11:25:40 by jim               #+#    #+#             */
/*   Updated: 2025/09/01 12:54:53 by jim              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "location.hpp"
#include <iostream>

Location::Location(const std::string &path) : _path(path){}
Location::~Location(){}

void Location::addParam(const ConfParam &param){
	_param.push_back(param);
}

std::string Location::getPath() const{
	return _path;
}

const std::vector<ConfParam> &Location::getParam() const{
	return _param;
}

void Location::print() const {
	std::cout << "Location "<<_path << " {" <<std::endl;
	for (size_t i = 0; i < _param.size(); i++){
		std::cout << " ";
		_param[i].print();
	}

	std::cout << "}"<<std::endl;
}
