#include "onConnection.hpp"

Conn::Conn()
: in(NULL)
, header_done(false)
, chunked(false)
, content_len(-1)
, body_have(0)
, headers_end(std::string::npos) 
{}

