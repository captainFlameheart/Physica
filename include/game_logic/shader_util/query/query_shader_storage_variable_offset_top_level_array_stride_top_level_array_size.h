#pragma once
#include "glad_glfw.h"

namespace game_logic::shader_util::query
{
	void query_shader_storage_variable_offset_top_level_array_stride_top_level_array_size
	(
		GLuint program, char const* name,
		GLint (&values)[3u]
	);
}
