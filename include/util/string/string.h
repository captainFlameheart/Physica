#pragma once
#include <string>

namespace util
{
	namespace string
	{
		template<typename T>
		std::string to_hex(T i);
	}
}

#include "util/string/string.tcc"
