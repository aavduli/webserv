#ifndef ERRORS_HPP
#define ERRORS_HPP

#include <string>

/* 
ERROR HANDLING
1. Syscall and other possible errors (invalid input, unexpected EOF) 
	-> return error codes and stop exec if needed + error enum list and related msg
2. Logical/program specific errors -> prevention with conditions, argument validation, flags
3. Improbable/exceptional errors (no more space) = exceptions + try blocks
4. Http errors -> status enum list of status codes and messages
 */

enum Error {
	E_OK,

	// Config file related
	E_INVALID_FILE,

	// Server related
	E_FAILED_CONNECTION,
	E_CLOSED_CONNECTION,

	// Parsing related
	E_INCOMPLETE_REQUEST,
	E_INVALID_REQUEST,

	E_INVALID_METHOD,
	E_INVALID_URI,
	E_INVALID_VERSION,
	E_INVALID_STATUS,
	E_INVALID_HOST,
	E_INVALID_PORT,
	E_INVALID_PATH,
	E_INVALID_HEADER,
	E_INVALID_CONTENT_LENGTH,

	// Request/response related

};

std::string	error_msg(Error e);

#endif //ERRORS_HPP