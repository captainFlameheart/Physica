#include "glad_glfw.h"

namespace game_state::bindings::shader_storage
{
	enum bindings : GLuint
	{
		fixed_data,
		uvec4_data,
		uvec2_data,
		uint_data,
		vec4_data,
		vec2_data,
		float_data,

		commands,

		count
	};

	constexpr GLuint bounding_volume_hierarchy = bindings::commands;
}
