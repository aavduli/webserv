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
	E_OK = 200,
	E_REDIRECT_PERMANENT = 301,
	E_REDIRECT_TEMPORARY = 302,
	E_BAD_REQUEST = 400,		// malformed request syntax, invalid request message framing, or deceptive request routing
	E_FORBIDDEN = 403,
	E_NOT_FOUND = 404,
	E_METHOD_NOT_ALLOWED = 405,
	E_LENGTH_REQUIRED = 411,
	E_PAYLOAD_TOO_LARGE = 413,
	E_URI_TOO_LONG = 414,
	E_UNSUPPORTED_MEDIA_TYPE = 415,
	E_EXPECTATION_FAILED = 417,
	E_TEAPOT = 418,
	E_TOO_MANY_REQUESTS = 429,
	E_HEADER_TOO_LARGE = 431,
	E_INTERNAL_SERVER_ERROR = 500,
	E_NOT_IMPLEMENTED = 501,	// server does not recognize the request method and is not capable of supporting it for any resource
	E_BAD_GATEWAY = 502,
	E_UNSUPPORTED_VERSION = 505,

};

std::string	status_msg(Status e);

#endif //STATUS_HPP
