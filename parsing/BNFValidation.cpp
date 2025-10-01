#include "Parsing.hpp"
#include "../console/console.hpp"

// tspecials = "(" | ")" | "<" | ">" | "@" | "," | ";" | ":" | "\" | <"> | "/" | "[" | "]" | "?" | "=" | "{" | "}" | SP | HT
bool	is_tspecial(const char c) {
	return (c == '(' || c == ')' || c == '<' || c =='>' || c == '@'
		|| c == ',' || c == ';' || c == ':' || c == '\\' || c == '\''
		|| c == '/' || c == '[' || c == ']' || c == '?' || c == '='
		|| c == '{' || c == '}' || is_lws(c));
}

// LWS = [CRLF] 1*( SP | HT )
bool	is_lws(const char c) {
	return (c == 32 || c == 9);
}

// CTL = <any US-ASCII control character (octets 0 - 31) and DEL (127)>
bool	is_ctl(const char c) {
	return (c <= 31 || c == 127);
}

// CHAR = <any US-ASCII character (octets 0 - 127)>
bool	is_char(const char c) {
	return (c);
}

// DIGIT = <any US-ASCII digit "0".."9">
bool	is_digit(const char c) {
	return (c >= '0' && c <= '9');
}

// token = 1*<any CHAR except CTLs or tspecials>
bool	is_token(const char c) {
	return (is_char(c) && !is_ctl(c) && !is_tspecial(c));
}

bool	is_token_str(const std::string& str) {

	for (size_t i = 0; i < str.length(); i++) {
		if (!is_token(str[i]))
			return false;
	}
	return true;
}

// TEXT = <any OCTET except CTLs, but including LWS>
bool	is_text_str(const std::string& str) {

	for (size_t i = 0; i < str.length(); i++) {
		if (is_ctl(str[i]))
			return false;
	}
	return true;
}
