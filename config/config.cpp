/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jim <jim@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/01 10:22:05 by jim               #+#    #+#             */
/*   Updated: 2025/09/01 13:00:38 by jim              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config.hpp"

void setColor(int textColor){
	std::cout << "\033["<< textColor << "m";
}

void resetColor(void){
	std::cout << "\033[0m";
}

ConfParam::ConfParam(const std::string &param, const std::string &comment):
	_param(param), _comment(comment){}

ConfParam::~ConfParam(){}

std::string ConfParam::getComment() const{
	return _comment;
}

std::string ConfParam::getParam() const{
	return _param;
}

void ConfParam::print() const{
	std::cout << " Parameters : ";
	setColor(31);
	std::cout <<_param;
	resetColor();
	if (!_comment.empty()){
		std::cout << " | Comment: ";
		setColor(32);
		std::cout << _comment;
		resetColor();
	}
	std::cout << std::endl;
}
