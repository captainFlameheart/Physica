#pragma once
#include "util/string/string.h"

namespace util::string
{
	template<typename T>
	std::string to_hex(T i)
	{
		std::stringstream stream;
		stream << "0x"
			<< std::setfill('0') << std::setw(2 * sizeof(T))
			<< std::hex << i;
		return stream.str();
	}
}
