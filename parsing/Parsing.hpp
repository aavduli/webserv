#ifndef PARSING_HPP
#define PARSING_HPP

#include <iostream>
#include <sstream>
#include <limits>
#include <string>
#include <vector>
#include <algorithm>

#include "../errors/errors.hpp"

// CONVERSION
size_t 						to_size_t(const char *number);
std::vector<std::string>	str_to_vect(const std::string& str, const std::string& del);

// STRING MANIPULATION
std::string	lower(const std::string& str);
std::string	trim(const std::string& str, std::vector<std::string>& to_trim);
std::string	trim_whitespaces(const std::string& str);

#endif //PARSING_HPP