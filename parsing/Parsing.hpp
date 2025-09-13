#ifndef PARSING_HPP
#define PARSING_HPP

#include <iostream>
#include <sstream>
#include <limits>
#include <string>
#include <vector>
#include <algorithm>

std::string	lower(const std::string& str);
std::string	trim(const std::string& str, std::vector<std::string>& to_trim);
std::vector<std::string>	str_to_vect(const std::string& str, const std::string& del);
size_t to_size_t(const char *number);

#endif //PARSING_HPP