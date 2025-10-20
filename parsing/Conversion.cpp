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

size_t to_size_t(const std::string& str) {

	size_t st;
	std::istringstream iss(str);
	iss >> st;
	if (iss.fail())
		return std::numeric_limits<size_t>::max();
	else
		return st;
}

int	hex_to_int(char c) {
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;
	if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	return 0;
}

std::string	urlDecode(const std::string& encoded) {

	std::string	decoded;

	for (size_t i = 0; i < encoded.size(); i++) {

		// %xx where xx is hexadecimal representation of a ASCII char
		if (encoded[i] == '%' && i + 2 < encoded.size()) {
			char hex1 = encoded[i + 1];
			char hex2 = encoded[i + 2];
			
			if (is_hex_digit(hex1) && is_hex_digit(hex2)) {
				int hex_val = (hex_to_int(hex1) << 4) | hex_to_int(hex2);
				decoded += static_cast<char>(hex_val);
				i += 2;
			}
			else
				decoded += encoded[i];
		}
		else if (encoded[i] == '+')
			decoded += ' ';
		else
			decoded += encoded[i];
	}
	return decoded;
}