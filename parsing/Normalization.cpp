#include "Parsing.hpp"
#include "../console/console.hpp"

bool	move_past_char(size_t* pos, const std::string& str, const std::string del) {
	if (*pos >= str.size() || str[*pos] != del[0])
		return false;
	(*pos)++;
	return true;
}

std::string trim_whitespaces(const std::string& str) {
	std::string whitespaces = " \t\r\n\v\f";
	size_t start = str.find_first_not_of(whitespaces);
	if (start == std::string::npos)
		return "";
	size_t end = str.find_last_not_of(whitespaces);
	return str.substr(start, end - start + 1);
}

std::string	trim_lws(const std::string& str) {
	
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
