#include "MessageParser.hpp"
#include "../data/HttpRequest.hpp"
#include "../data/HttpResponse.hpp"

MessageParser::MessageParser(const WebservConfig& config) : _config(config), _state(s_msg_init), _current_pos(0), _content_length(0) {}

MessageParser::MessageParser(const MessageParser& rhs) : _config(rhs._config), _state(rhs._state), _raw_data(rhs._raw_data), _current_pos(rhs._current_pos), _content_length(rhs._content_length) {}

MessageParser& MessageParser::operator=(const MessageParser& rhs) {
	if (this != &rhs) {
		_state = rhs._state;
		_raw_data = rhs._raw_data;
		_current_pos = rhs._current_pos;
		_content_length = rhs._content_length;
	}
	return *this;
}

MessageParser::~MessageParser() {}

State	MessageParser::getState() const {
	return _state;
}

size_t	MessageParser::getContentLength() const {
	return _content_length;
}

void	MessageParser::setState(State state) {
	_state = state;
}

// Check for end of headers and CRLF line + compare body length to Content-Length
bool	MessageParser::is_complete_request(const std::string& buffer) {

	size_t	head_end = buffer.find("\r\n\r\n");
	if (head_end == std::string::npos)
		return false;
		
	_content_length = extract_content_length(buffer);
	if (_state == s_req_invalid_content_length)
		return false;

	size_t body_start = head_end + 4;
	size_t body_len = buffer.size() - body_start;
	if (body_len && !_content_length) {
		// 400 (bad request) if it cannot determine the length of the message OR
		// 411 (length required) if it wishes to insist on receiving a valid Content-Length
		console::log("Status 400/411", ERROR, ALL);
		return false;
	}
	return body_len >= _content_length;
}

size_t	MessageParser::extract_content_length(const std::string& buffer) {

	size_t	pos = buffer.find("Content-Length:");
	if (pos == std::string::npos) {
		console::log("No \"Content-Length\" header field", WARNING, AH);
		return 0;
	}
	char const* digits = "0123456789";
	size_t len_start = buffer.find_first_of(digits, pos);
	if (len_start == std::string::npos) {
		console::log("Missing \"Content-Length\" value", ERROR, ALL);
		_state = s_req_invalid_content_length;
		return 0;
	}
	std::size_t len_end = buffer.find_first_not_of(digits, len_start);
	if (len_end == std::string::npos) {
		console::log("Invalid \"Content-Length\" value", ERROR, ALL);
		_state = s_req_invalid_content_length;
		return 0;
	}
	std::string nb = buffer.substr(len_start, len_end);
	size_t content_len = to_size_t(nb);
	size_t max_body_size = to_size_t(_config.getDirective("max_body_size"));	// config use correct?
	if (content_len > max_body_size) {
		console::log("\"Content-Length\" value > MAX_CONTENT_LENGTH", ERROR, ALL);
		_state = s_req_invalid_content_length;
		return 0;
	}
	return content_len;
}
