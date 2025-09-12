#include "Parsing.hpp"

std::string	lower(const std::string& str) {
	if (str.empty())
		return str;
	std::string normalized = str;
	std::transform(str.begin(), str.end(), normalized.begin(), ::tolower);
	return normalized;
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

std::vector<std::string>	stringToVector(const std::string& str, const std::string& del) {

	std::vector<std::string>	v;

	size_t	current_pos = 0;
	size_t	found = str.find(del, current_pos);
	while (found != std::string::npos) {
		v.push_back(str.substr(current_pos, found - current_pos));
		current_pos = found + 1;
		found = str.find(del, current_pos);
	}
	return v;
}