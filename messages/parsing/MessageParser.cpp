#include "MessageParser.hpp"
#include "../data/HttpRequest.hpp"
#include "../data/HttpResponse.hpp"

MessageParser::MessageParser() : _state(s_msg_init), _current_pos(0) {
	console::log("MessageParser Constructor", DEBUG);
}

MessageParser::MessageParser(const MessageParser& rhs) : _state(rhs._state), _raw_data(rhs._raw_data), _current_pos(rhs._current_pos) {
	console::log("MessageParser copy constructor", DEBUG);
}

MessageParser& MessageParser::operator=(const MessageParser& rhs) {
	console::log("MessageParser assignment operator", DEBUG);
	if (this != &rhs) {
		_state = rhs._state;
		_raw_data = rhs._raw_data;
		_current_pos = rhs._current_pos;
	}
	return *this;
}

MessageParser::~MessageParser() {
	console::log("MessageParser destructor", DEBUG);
}

State	MessageParser::getState() const {
	return _state;
}

void	MessageParser::setState(State state) {
	_state = state;
}

// Check for end of headers and CRLF line + compare body length to Content-Length
bool	MessageParser::is_complete_request(const std::string& buffer) {

	size_t	head_end = buffer.find("\r\n\r\n");
	if (head_end == std::string::npos)
		return false;
		
	size_t content_len = get_content_length(buffer);
	if (_state == s_req_invalid_content_length)
		return false;

	size_t body_start = head_end + 4;
	size_t body_len = buffer.size() - body_start;
	if (body_len && !content_len) {
		// 400 (bad request) if it cannot determine the length of the message OR
		// 411 (length required) if it wishes to insist on receiving a valid Content-Length
		console::log("Status 400/411", ERROR);
		return false;
	}
	return body_len >= content_len;
}

size_t	MessageParser::get_content_length(const std::string& buffer) {

	size_t	pos = buffer.find("Content-Length:");
	if (pos == std::string::npos) {
		console::log("Missing \"Content-Length\" header field", ERROR);
		// _state = s_req_invalid_content_length;
		// 400 (bad request) if it cannot determine the length of the message OR
		// 411 (length required) if it wishes to insist on receiving a valid Content-Length
		return 0;
	}
	char const* digits = "0123456789";
	size_t len_start = buffer.find_first_of(digits, pos);
	if (len_start == std::string::npos) {
		console::log("Missing \"Content-Length\" value", ERROR);
		_state = s_req_invalid_content_length;
		return 0;
	}
	std::size_t len_end = buffer.find_first_not_of(digits, len_start);
	if (len_end == std::string::npos) {
		console::log("Invalid \"Content-Length\" value", ERROR);
		_state = s_req_invalid_content_length;
		return 0;
	}
	std::string nb = buffer.substr(len_start, len_end);
	size_t	content_len = to_size_t(nb.c_str());
	if (content_len > MAX_CONTENT_LENGTH) {
		console::log("\"Content-Length\" value > MAX_CONTENT_LENGTH", ERROR);
		_state = s_req_invalid_content_length;
		return 0;
	}
	return content_len;
}
