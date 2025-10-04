#ifndef PARSING_HPP
#define PARSING_HPP

#include <iostream>
#include <sstream>
#include <limits>
#include <sys/stat.h>
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

// Files and paths
bool	is_valid_path(const std::string& path);
bool	is_valid_file_path(const std::string& path);
bool	is_directory(const std::string& path);

std::string	extract_relative_path(const std::string& full_path, const std::string& location_prefix);
std::string	build_full_path(const std::string& root, const std::string& relative_path);
std::string	resolve_index_file(const std::string& directory_path, const std::string& index_file);
bool		contains_traversal(const std::string& path);
std::string canonicalize_path(const std::string& path);
bool		is_within_root(const std::string& path, const std::string& document_root);

#endif //PARSING_HPP