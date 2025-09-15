#include "MessageStreams.hpp"

stream_msg::stream_msg() : simple(EOF), has_eof(false), is_chunked(false), response_done(false) {}
