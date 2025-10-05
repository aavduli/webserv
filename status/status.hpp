#ifndef STATUS_HPP
#define STATUS_HPP

#include <string>

/* 
ERROR HANDLING
1. Syscall and other possible errors (invalid input, unexpected EOF) 
	-> return error codes and stop exec if needed + error enum list and related msg
2. Logical/program specific errors -> prevention with conditions, argument validation, flags
3. Improbable/exceptional errors (no more space) = exceptions + try blocks
4. Http errors -> status enum list of status codes and messages
 */

enum Status {
	E_INIT,
	E_OK,

	// Config file related
	E_INVALID_FILE,

	// Server related
	E_FAILED_CONNECTION,
	E_CLOSED_CONNECTION,

	E_REDIRECT_PERMANENT = 301,
	E_REDIRECT_TEMPORARY = 302,

	// Parsing related
	E_PARSING_DONE,
	E_INCOMPLETE_REQUEST,
	E_INVALID_REQUEST,
	E_INVALID_METHOD,
	E_INVALID_URI,
	E_INVALID_VERSION,

	E_BAD_REQUEST = 400,
	E_FORBIDDEN = 403,
	E_NOT_FOUND = 404,
	E_METHOD_NOT_ALLOWED = 405,
	E_ENTITY_TOO_LARGE = 413,
	E_UNSUPPORTED_MEDIA_TYPE = 415,
	E_EXPECTATION_FAILED = 417,
	E_UNSUPPORTED_VERSION = 505,
	E_INVALID_STATUS,
	E_INVALID_HOST,
	E_EMPTY_HEADER_HOST,
	E_INVALID_PORT,
	E_INVALID_PATH,
	E_PATH_TRAVERSALS,
	E_PATH_ESCAPES_ROOT,
	E_INVALID_HEADER,
	E_INVALID_CONTENT_LENGTH,
	E_INVALID_TRANSFER_ENCODING,
	E_MISSING_CONTENT_TYPE,
	E_TOO_MANY_HEADERS,
	E_INVALID_CONNECTION,
	E_INVALID_REDIRECT,


};

std::string	status_msg(Status e);

#endif //STATUS_HPP