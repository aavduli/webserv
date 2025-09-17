#ifndef MESSAGESTREAMS_HPP
#define MESSAGESTREAMS_HPP

#include <iostream>
#include <fstream>

#include "../errors/errors.hpp"

class s_msgStream {
	protected:
		std::string simplerequest;
		bool reqFinsh;
	public:
		s_msgStream();
		~s_msgStream();
		std::string reqHandler(std::string request);
};

#endif
