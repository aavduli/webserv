#include "status.hpp"

std::string	status_msg(Status e) {
	switch (e) {
		case E_INIT: return "Initialization";
		case E_OK: return "Success";
		case E_REDIRECT_PERMANENT: return "Moved Permanently";
		case E_REDIRECT_TEMPORARY: return "Found (Moved Temporarily)";
		case E_BAD_REQUEST: return "Bad Request";
		case E_FORBIDDEN: return "Forbidden";
		case E_NOT_FOUND: return "Not Found";
		case E_METHOD_NOT_ALLOWED: return "Method Not Allowed";
		case E_LENGTH_REQUIRED: return "Length Required";
		case E_PAYLOAD_TOO_LARGE: return "Payload Too Large";
		case E_URI_TOO_LONG: return "URI Too Long";
		case E_UNSUPPORTED_MEDIA_TYPE: return "Unsupported Media Type";
		case E_EXPECTATION_FAILED: return "Expectation Failed";
		case E_TEAPOT: return "I'm a teapot";
		case E_TOO_MANY_REQUESTS: return "Too Many Requests";
		case E_HEADER_TOO_LARGE: return "Request Header Fields Too Large";
		case E_INTERNAL_SERVER_ERROR: return "Internal Server Error";
		case E_NOT_IMPLEMENTED: return "Not Implemented";
		case E_BAD_GATEWAY: return "Bad Gateway";
		case E_UNSUPPORTED_VERSION: return "HTTP Version Not Supported";
		default: return "Unknown error";
	}
}