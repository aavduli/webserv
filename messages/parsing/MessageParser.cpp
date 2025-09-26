#include "MessageParser.hpp"
#include "../data/HttpRequest.hpp"
#include "../data/HttpResponse.hpp"

MessageParser::MessageParser() : _state(s_msg_init), _current_pos(0) {}

MessageParser::MessageParser(const MessageParser& rhs) : _state(rhs._state), _raw_data(rhs._raw_data), _current_pos(rhs._current_pos) {}

MessageParser& MessageParser::operator=(const MessageParser& rhs) {
	if (this != &rhs) {
		_state = rhs._state;
		_raw_data = rhs._raw_data;
		_current_pos = rhs._current_pos;
	}
	return *this;
}

MessageParser::~MessageParser() {}

State	MessageParser::getState() const {
	return _state;
}

void	MessageParser::setState(State state) {
	_state = state;
}
