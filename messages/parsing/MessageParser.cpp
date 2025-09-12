#include "MessageParser.hpp"
#include "../data/HttpRequest.hpp"
#include "../data/HttpResponse.hpp"

MessageParser::MessageParser() : _state(s_msg_init), _current_pos(0), _request(NULL), _response(NULL) {
	console::log("MessageParser Constructor", DEBUG);
}

MessageParser::MessageParser(const MessageParser& rhs) : _state(rhs._state), _raw_data(rhs._raw_data), _current_pos(rhs._current_pos) {
	console::log("MessageParser copy constructor", DEBUG);
	if (rhs._request)
		_request = new HttpRequest(*rhs._request);
	else
		_request = NULL;
	if (rhs._response)
		_response = new HttpResponse(*rhs._response);
	else
		_response = NULL;
}

MessageParser& MessageParser::operator=(const MessageParser& rhs) {
	console::log("MessageParser assignment operator", DEBUG);
	if (this != &rhs) {
		_state = rhs._state;
		_raw_data = rhs._raw_data;
		_current_pos = rhs._current_pos;
		delete _request;
		delete _response;
		if (rhs._request)
			_request = new HttpRequest(*rhs._request);
		else
			_request = NULL;
		if (rhs._response)
			_response = new HttpResponse(*rhs._response);
		else
			_response = NULL;
	}
	return *this;
}

MessageParser::~MessageParser() {
	console::log("MessageParser destructor", DEBUG);
	if (_request)
		delete _request;
	if (_response)
		delete _response;
}
