#include "errors.hpp"

std::string	error_msg(Error e) {
	switch (e) {
		case E_OK: return "Success";
		case E_INVALID_FILE: return "Invalid file";
		case E_FAILED_CONNECTION: return "Failed connection";
		case E_CLOSED_CONNECTION: return "Closed connection";
		case E_INCOMPLETE_REQUEST: return "Incomplete request";
		case E_INVALID_REQUEST: return "Invalid request";
		case E_INVALID_METHOD: return "Invalid method";
		case E_INVALID_URI: return "Invalid URI";
		case E_INVALID_VERSION: return "Invalid HTTP version";
		case E_INVALID_STATUS: return "Invalid status";
		case E_INVALID_HOST: return "Invalid host";
		case E_INVALID_PORT: return "Invalid port";
		case E_INVALID_PATH: return "Invalid path";
		case E_INVALID_HEADER: return "Invalid header";
		case E_INVALID_CONTENT_LENGTH: return "Invalid content length";
		default: return "Unknown error";
	}
}