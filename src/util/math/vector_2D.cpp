#include "util/math/vector_2D.h"

namespace util::math
{
	std::ostream& operator<<(std::ostream& os, Vector_2D const& v)
	{
		return os << '(' << v.x << ", " << v.y << ')';
	}
}
