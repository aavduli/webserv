#include "status.hpp"

std::string	status_msg(Status e) {
	switch (e) {
		case E_INIT: return "Initialization";
		case E_OK: return "Success";
		case E_INVALID_FILE: return "Invalid file";
		case E_FAILED_CONNECTION: return "Failed connection";
		case E_CLOSED_CONNECTION: return "Closed connection";
		case E_INCOMPLETE_REQUEST: return "Incomplete request";
		case E_INVALID_REQUEST: return "Invalid request";
		case E_INVALID_METHOD: return "Invalid method";
		case E_METHOD_NOT_ALLOWED: return "Method not allowed";
		case E_INVALID_URI: return "Invalid URI";
		case E_INVALID_VERSION: return "Invalid HTTP version";
		case E_UNSUPPORTED_VERSION: return "HTTP version not supported";
		case E_INVALID_STATUS: return "Invalid status";
		case E_INVALID_HOST: return "Invalid host";
		case E_EMPTY_HEADER_HOST: return "Host header is required";
		case E_INVALID_PORT: return "Invalid port";
		case E_INVALID_PATH: return "Invalid path";
		case E_PATH_TRAVERSALS: return "Path traversal attempt detected";
		case E_PATH_ESCAPES_ROOT: return "Path escapes document root";
		case E_NOT_FOUND: return "Not found";
		case E_INVALID_HEADER: return "Invalid header";
		case E_ENTITY_TOO_LARGE: return "Request entity too large";
		case E_INVALID_CONTENT_LENGTH: return "Invalid content length";
		case E_INVALID_TRANSFER_ENCODING: return "Invalid transfer encoding";
		case E_MISSING_CONTENT_TYPE: return "Content-Type header required";
		case E_UNSUPPORTED_MEDIA_TYPE: return "Unsupported media type";
		case E_TOO_MANY_HEADERS: return "Too many headers";
		case E_EXPECTATION_FAILED: return "Expectation failed";
		case E_INVALID_CONNECTION: return "Invalid connection header";
		case E_INVALID_REDIRECT: return "Invalid redirect configuration";
		case E_REDIRECT_PERMANENT: return "Moved permanently";
		case E_REDIRECT_TEMPORARY: return "Found (temporary redirect)";
		case E_BAD_REQUEST: return "Bad request";
		default: return "Unknown error";
	}
}