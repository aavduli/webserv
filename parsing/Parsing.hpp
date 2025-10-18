#ifndef PARSING_HPP
#define PARSING_HPP

#include <iostream>
#include <sstream>
#include <limits>
#include <sys/stat.h>
#include <string>
#include <vector>
#include <algorithm>

#include "../status/status.hpp"

// CONVERSION

template<typename T> std::string nb_to_string(T value) {
	std::stringstream ss;
	ss << value;
	return ss.str();
}
size_t 						to_size_t(std::string str);
std::vector<std::string>	str_to_vect(const std::string& str, const std::string& del);
std::vector<std::string>	str_to_vect_exept_between(const std::string& str, const std::string& del, const std::string& open, const std::string& close);
int							hex_to_int(char c);

// STRING MANIPULATION
bool		move_past_char(size_t* pos, const std::string& str, const std::string del);
std::string	trim(const std::string& str, std::vector<std::string>& to_trim);
std::string	trim_whitespaces(const std::string& str);
std::string	trim_lws(const std::string& str);

// VALIDATION
bool	is_tspecial(const char c);
bool	is_lws(const char c);
bool	is_ctl(const char c);
bool	is_char(const char c);
bool	is_digit(const char c);
bool	is_token(const char c);
bool	is_token_str(const std::string& str);
bool	is_text_str(const std::string& str);
bool	is_hex_digit(char c);

// Files and paths
bool				is_valid_path(const std::string& path);
bool				is_valid_file_path(const std::string& path);
bool				is_directory(const std::string& path);
bool				is_python_CGI(const std::string& path);
std::string			get_file_extension(const std::string& path);

std::string			remove_suffix(const std::string& str, const std::string& suffix);
std::string			remove_prefix(const std::string& path, const std::string& prefix);
std::string			build_full_path(const std::string& document_root, const std::string& path);
bool				contains_traversal(const std::string& path);
std::string			canonicalize_path(const std::string& path);
bool				is_within_root(const std::string& path, const std::string& document_root);

#endif //PARSING_HPP