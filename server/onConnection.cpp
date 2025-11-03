#include "onConnection.hpp"

Conn::Conn()
	: in()
	, header_done(false)
	, chunked(false)
	, content_len(-1)
	, body_have(0)
	, headers_end(std::string::npos)
	, lastActivity(time(NULL))
	, serverFd(-1)
	, clientPort(-1)
	, waitingForCgi(false)
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
		c.serverFd = -1;
		c.clientPort = -1;
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

bool onConn::try_mark_headers(Conn &c) {
    if (c.header_done) return true;

    size_t maxHeaderSize = MAX_HEADER_BYTES;

    if (c.in.size() > maxHeaderSize && c.in.find("\r\n\r\n") == std::string::npos) {
        console::log("Headers too large for server FD: ", c.serverFd, ERROR);
        return false;
    }

    size_t he = headers_end_pos(c.in);
    if (he != std::string::npos) {
        c.header_done = true;
        c.headers_end = he;
        inspect_headers_minimally(c);
        c.body_have = (c.in.size() > he) ? (c.in.size() - he) : 0;
        return true;
    }
    return false;
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
	const std::string key = "transfer-encoding:";
	size_t p = headers.find(key);
	if (p != std::string::npos) {
		size_t eol = headers.find("\r\n", p);
		std::string line = (eol == std::string::npos) ? headers.substr(p) : headers.substr(p, eol - p);
		if (line.find("chunked") != std::string::npos) c.chunked = true;
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
		onConn::updateActivity(c);
		if (c.in.size() >= req_end)
			return true;
		return false;
	}
	req_end = c.headers_end;
	return true;
}

std::string onConn::extractHostHeader(const Conn& c) {
	if (!c.header_done) return "";

	const std::string headers = c.in.substr(0, c.headers_end);
	size_t host_start = headers.find("Host:");
	if (host_start == std::string::npos) {
		host_start = headers.find("host:");
	}
	if (host_start == std::string::npos) return "";

	host_start += 5;
	while (host_start < headers.size() && (headers[host_start] == ' ' || headers[host_start] == '\t')) {
		host_start++;
	}

	size_t host_end = headers.find("\r\n", host_start);
	if (host_end == std::string::npos) return "";

	std::string host = headers.substr(host_start, host_end - host_start);
	while (!host.empty() && (host[host.size() - 1] == ' ' || host[host.size() - 1] == '\t')) {
		host.erase(host.size() - 1, 1);
	}

	return host;
}
