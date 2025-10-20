#include "onConnection.hpp"

Conn::Conn()
	: in()
	, header_done(false)
	, chunked(false)
	, content_len(-1)
	, body_have(0)
	, headers_end(std::string::npos)
	, lastActivity(time(NULL))
{}

onConn::onConn() {}

onConn::~onConn() {}

bool onConn::onDiscon(Conn& c,bool alive, size_t endpos) {
	if (!alive) {
		c.in.erase(0, endpos);
		c.header_done = false;
		c.chunked = false;
		c.content_len = -1;
		c.body_have = 0;
		c.headers_end = std::string::npos;
		c.lastActivity = time(NULL);
		return true;
	}
	return false;
}
// 
// static std::string to_lower(std::string s) {
// 	for (size_t i = 0; i < s.size(); ++i)
// 		s[i] = static_cast<char>(std::tolower(s[i]));
// 	return s;
// }

bool onConn::isTimedOut(Conn& c, time_t currentTime, int timeOutSeconds) {
	return (currentTime - c.lastActivity) >= timeOutSeconds;
}

void onConn::updateActivity(Conn& currentConn) {
	currentConn.lastActivity = time(NULL);
}

size_t onConn::headers_end_pos(const std::string &buff) {
	size_t pos = buff.find("\r\n\r\n");
	return (pos == std::string::npos) ? pos : pos + 4;
}

void onConn::try_mark_headers(Conn &c) {
	if (c.header_done) return;
	if (c.in.size() > MAX_HEADER_BYTES && c.in.find("\r\n\r\n") == std::string::npos) {
		return ;
	}
	size_t he = headers_end_pos(c.in);
	if (he != std::string::npos) {
		c.header_done = true;
		c.headers_end = he;
		inspect_headers_minimally(c);
		c.body_have = (c.in.size() > he) ? (c.in.size() - he) : 0;
	}
}

void onConn::inspect_headers_minimally (Conn &c) {
	c.chunked = false;
	c.content_len = -1;

	const std::string headers = c.in.substr(0, c.headers_end);
	{
		const std::string key = "Content-Length:";
		size_t p = headers.find(key);
		if (p != std::string::npos) {
			p += key.size();
			while (p < headers.size() && std::isspace(headers[p])) ++p;
			size_t q = p;
			while (q < headers.size() && std::isdigit(headers[q])) ++q;
			if (q > p) {
				long v = std::strtol(headers.substr(p, q - p).c_str(), 0, 10);
				if (v>= 0) c.content_len = v;
			}
		}
	}
	{
		const std::string key = "transfer-encoding:";
		size_t p = headers.find(key);
		if (p != std::string::npos) {
			size_t eol = headers.find("\r\n", p);
			std::string line = (eol == std::string::npos) ? headers.substr(p) : headers.substr(p, eol - p);
			if (line.find("chunked") != std::string::npos) c.chunked = true;
		}
	}
}

bool onConn::chunked_complete(const std::string& body, size_t &cut_after) {
	if (body.size() >= 5 && body.compare(body.size() - 5, 5, "0\r\n\r\n") == 0) {
		cut_after = body.size();
		return true;
	}
	size_t p = body.find("\r\n0\r\n\r\n");
	if (p != std::string::npos) {
		cut_after = p + 7;
		return true;
	}
	return false;
}
// TODO check for post request by searching "--\r\n\r\n" if content-type = multipart/form data == chunked request

bool onConn::update_and_ready(Conn& c, size_t &req_end) {
	req_end = 0;

	try_mark_headers(c);
	if (!c.header_done) return false;

	if (c.in.size() > c.headers_end) {
		c.body_have = c.in.size() - c.headers_end;
	}
	else
		c.body_have = 0;
	if (c.chunked) {
		const std::string body = c.in.substr(c.headers_end);
		size_t cut = 0;
		if (!chunked_complete(body, cut)) return false;
		req_end = c.headers_end + cut;
		return true;
	}
	if (c.content_len >= 0) {
		const size_t need = static_cast<size_t>(c.content_len);
		req_end = c.headers_end + need;
		return true;
	}
	req_end = c.headers_end;
	return true;
}
