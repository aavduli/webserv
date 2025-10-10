#include "Parsing.hpp"
#include "../console/console.hpp"

std::vector<std::string>	str_to_vect(const std::string& str, const std::string& del) {

	std::vector<std::string>	v;

	size_t	current_pos = 0;
	if (!del.empty()) {
		size_t	found = str.find(del, current_pos);
		while (found != std::string::npos) {
			v.push_back(trim_whitespaces(str.substr(current_pos, found - current_pos)));
			current_pos = found + 1;
			found = str.find(del, current_pos);
		}
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

size_t to_size_t(std::string str) {

	size_t st;
	std::istringstream iss(str);
	iss >> st;
	if (iss.fail())
		return std::numeric_limits<size_t>::max();
	else
		return st;
}

template<typename T>
std::string nb_to_string(T value) {
	std::ostringstream stream;
	stream << value;
	return stream.str();
}