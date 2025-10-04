#include "MessageParser.hpp"
#include "../data/HttpRequest.hpp"
#include "../data/HttpResponse.hpp"

MessageParser::MessageParser() : _status(E_INIT), _current_pos(0) {}

MessageParser::MessageParser(const MessageParser& rhs) : _raw_data(rhs._raw_data), _current_pos(rhs._current_pos) {}

MessageParser& MessageParser::operator=(const MessageParser& rhs) {
	if (this != &rhs) {
		_raw_data = rhs._raw_data;
		_current_pos = rhs._current_pos;
	}
	return *this;
}

MessageParser::~MessageParser() {}

Status	MessageParser::getStatus() const {
	return _status;
}