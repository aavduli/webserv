#include "Parsing.hpp"
#include "../console/console.hpp"

std::string	lower(const std::string& str) {
	if (str.empty())
		return str;
	std::string normalized = str;
	std::transform(str.begin(), str.end(), normalized.begin(), ::tolower);
	return normalized;
}

std::string	trim_whitespaces(const std::string& str) {
	
	if (str.empty())
		return str;

	std::vector<std::string> to_trim;
	to_trim.push_back(" ");
	to_trim.push_back("\t");
	
	std::string	trimmed = str;
	std::vector<std::string>::iterator	it = to_trim.begin();
	std::vector<std::string>::iterator	ite = to_trim.end();

	for (int i = 0; it != ite; ++it, i++) {
		size_t	start = trimmed.find_first_not_of(to_trim[i]);
		if (start != std::string::npos) {
			size_t	end = trimmed.find_last_not_of(to_trim[i]);
			size_t	size = end - start + 1;
			trimmed = trimmed.substr(start, size);
		}
	}
	return trimmed;
}

std::string	trim(const std::string& str, std::vector<std::string>& to_trim) {
	
	if (str.empty())
		return str;
	
	std::string	trimmed = str;
	std::vector<std::string>::iterator	it = to_trim.begin();
	std::vector<std::string>::iterator	ite = to_trim.end();

	for (int i = 0; it != ite; ++it, i++) {
		size_t	start = trimmed.find_first_not_of(to_trim[i]);
		if (start != std::string::npos) {
			size_t	end = trimmed.find_last_not_of(to_trim[i]);
			size_t	size = end - start + 1;
			trimmed = trimmed.substr(start, size);
		}
	}
	return trimmed;
}

std::vector<std::string>	str_to_vect(const std::string& str, const std::string& del) {

	std::vector<std::string>	v;

	size_t	current_pos = 0;
	size_t	found = str.find(del, current_pos);
	while (found != std::string::npos) {
		v.push_back(trim_whitespaces(str.substr(current_pos, found - current_pos)));
		current_pos = found + 1;
		found = str.find(del, current_pos);
	}
	v.push_back(trim_whitespaces(str.substr(current_pos, str.size() - current_pos)));
	return v;
}

std::vector<std::string>	str_to_vect_exept_between(const std::string& str, const std::string& del, const std::string& open, const std::string& close) {

	std::vector<std::string>	v;
	size_t current_pos = 0;
	size_t found = str.find(del, current_pos);
	size_t open_pos = str.find(open, current_pos);
	size_t close_pos = str.find(close, open_pos);

	while (found != std::string::npos) {
		while (open_pos != std::string::npos && close_pos != std::string::npos) {
			if (found > close_pos) {
				open_pos = str.find(open, close_pos + 1);
				close_pos = str.find(close, open_pos);
			}
			while (found > open_pos && found < close_pos)
				found = str.find(del, close_pos + 1);
		}
		if (found == std::string::npos)
			break;
		v.push_back(trim_whitespaces(str.substr(current_pos, found - current_pos)));
		current_pos = found + 1;
		found = str.find(del, current_pos);
	}
	v.push_back(trim_whitespaces(str.substr(current_pos, str.size() - current_pos)));
	return v;
}

size_t to_size_t(const char *number) {

	size_t st;
	std::istringstream iss(number);
	iss >> st;
	if (iss.fail())
		return std::numeric_limits<size_t>::max();
	else
		return st;
}