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
size_t 						to_size_t(std::string str);
std::vector<std::string>	str_to_vect(const std::string& str, const std::string& del);
std::vector<std::string>	str_to_vect_exept_between(const std::string& str, const std::string& del, const std::string& open, const std::string& close);

// STRING MANIPULATION
bool		move_past_char(size_t* pos, const std::string& str, const std::string del);
std::string	lower(const std::string& str);
std::string	trim(const std::string& str, std::vector<std::string>& to_trim);
std::string	trim_whitespaces(const std::string& str);
std::string	trim_lws(const std::string& str);

// BNF VALIDATION
bool	is_tspecial(const char c);
bool	is_lws(const char c);
bool	is_ctl(const char c);
bool	is_char(const char c);
bool	is_digit(const char c);
bool	is_token(const char c);
bool	is_token_str(const std::string& str);
bool	is_text_str(const std::string& str);

#endif //PARSING_HPP