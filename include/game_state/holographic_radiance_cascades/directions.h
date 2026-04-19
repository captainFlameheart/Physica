#include "glad_glfw.h"

namespace game_state::holographic_radiance_cascades::directions
{
	enum directions : GLuint
	{
		east,
		west,
		north,
		south,

		count
	};

	constexpr GLuint flatten[2u][2u]
	{
		{ east, west },
		{ north, south }
	};
}
