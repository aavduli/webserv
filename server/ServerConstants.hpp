#ifndef SERVER_CONSTANTS_HPP
# define SERVER_CONSTANTS_HPP

namespace ServerConstants {
	// Buffer sizes
	static const size_t BUFFER_SIZE = 8192;
	static const size_t MAX_REQUEST_SIZE = 1048576;  // 1MB
	static const size_t MAX_HEADER_SIZE = 16384;     // 16KB
	
	// Connection limits  
	static const size_t DEFAULT_MAX_CONNECTIONS = 1000;
	static const size_t DEFAULT_MAX_EVENTS = 1024;
	
	// Timeouts (in seconds)
	static const int CONNECTION_TIMEOUT = 60;
	static const int TIMEOUT_CHECK = 10;
	static const int READ_TIMEOUT = 30;
	static const int WRITE_TIMEOUT = 30;
}

#endif
