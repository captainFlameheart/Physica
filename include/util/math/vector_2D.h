#pragma once
#include <GLFW/glfw3.h>
#include <iostream>

namespace util::math
{
	struct Vector_2D
	{
		GLint x;
		GLint y;
	};

	std::ostream& operator<<(std::ostream& os, Vector_2D const& v);
}
