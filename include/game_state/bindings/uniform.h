#include "glad_glfw.h"

namespace game_state::bindings::uniform
{
	enum bindings : GLuint
	{
		fixed_data,
		private_input,
		timing_configuration,
		timing_metadata,

		count
	};
}
