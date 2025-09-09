#include "Parsing.hpp"

std::string	toLowerStr(const std::string& str) {
	if (str.empty())
		return str;
	std::string normalized = str;
	std::transform(str.begin(), str.end(), normalized.begin(), ::tolower);
	return normalized;
}
