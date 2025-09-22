/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileReader.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jim <jim@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/15 11:43:45 by jim               #+#    #+#             */
/*   Updated: 2025/09/22 17:27:30 by jim              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "FileReader.hpp"
#include "ParsingUtils.hpp"
#include "../console/console.hpp"
#include <fstream>
#include <iostream>

std::vector<std::string> FileReader::readLines(const std::string& filepath) const{
	std::vector<std::string> lines;

	ParsingUtils _utils;

	std::ifstream file(filepath.c_str());

	console::log(filepath.c_str(), WARNING);

	if (!file.is_open()){
		std::cerr << " Error, cant open fille " <<filepath << std::endl;
		return lines;
	}

	std::string line;
	while(std::getline(file, line)){
		lines.push_back(_utils.trim(line));
	}

	return lines;
}

bool FileReader::fileExist(const std::string& filepath) const{
	std::ifstream file(filepath.c_str());
	return file.good();
}
