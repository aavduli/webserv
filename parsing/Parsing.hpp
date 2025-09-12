#ifndef PARSING_HPP
#define PARSING_HPP

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

std::string	lower(const std::string& str);
std::string	trim(const std::string& str, std::vector<std::string>& to_trim);
std::vector<std::string>	stringToVector(const std::string& str, const std::string& del);

#endif //PARSING_HPP