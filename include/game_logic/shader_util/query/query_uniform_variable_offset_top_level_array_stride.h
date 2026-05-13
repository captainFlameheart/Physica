#pragma once
#include "glad_glfw.h"

namespace game_logic::shader_util::query
{
	void query_uniform_variable_offset_top_level_array_stride
	(
		GLuint program, char const* name,
		GLint (&values)[2u]
	);
}
