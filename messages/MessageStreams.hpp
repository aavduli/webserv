#ifndef MESSAGESTREAMS_HPP
#define MESSAGESTREAMS_HPP

#include <iostream>
#include <fstream>

#include "../errors/errors.hpp"

class s_msgStream {
	protected:
		std::string acc;
	public:
		s_msgStream();
		~s_msgStream();
		std::vector<std::string> feed(const char* data, size_t n);
};

#endif
