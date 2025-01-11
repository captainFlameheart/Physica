#include "util/string.h"

template<typename T>
std::string util::string::to_hex(T i)
{
	std::stringstream stream;
	stream << "0x"
		<< std::setfill('0') << std::setw(2 * sizeof(T))
		<< std::hex << i;
	return stream.str();
}
