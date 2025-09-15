/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   file_reader.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jim <jim@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/15 11:43:45 by jim               #+#    #+#             */
/*   Updated: 2025/09/15 11:56:47 by jim              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "file_reader.hpp"
#include <fstream>
#include <iostream>

std::vector<std::string> FileReader::readLines(const std::string& filepath) const{
	std::vector<std::string> lines;
	std::ifstream file(filepath.c_str());

	if (!file.is_open()){
		std::cerr << " Error, cant open fille " <<filepath << std::endl;
		return lines;
	}

	std::string line;
	while(std::getline(file, line)){
		lines.push_back(trim(line));
	}

	return lines;
}

std::string FileReader::trim(const std::string& str) const{
	size_t start = str.find_first_not_of(" \t");
	size_t end = str.find_last_not_of(" \t");
	if (start == std::string::npos || end == std::string::npos)
		return "";
	return str.substr(start, end - start + 1);
}

bool FileReader::fileExist(const std::string& filepath) const{
	std::ifstream file(filepath.c_str());
	return file.good();
}
