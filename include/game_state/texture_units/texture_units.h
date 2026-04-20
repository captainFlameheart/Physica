#include "glad_glfw.h"

namespace game_state::texture_units
{
	enum texture_units : GLuint
	{
		source_image,
		skycircle,

		rays,
		upper_cascade_fluence,
		fluence,

		count
	};
}
