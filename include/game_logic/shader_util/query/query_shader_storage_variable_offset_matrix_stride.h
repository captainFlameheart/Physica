#pragma once
#include "glad_glfw.h"

namespace game_logic::shader_util::query
{
	void query_shader_storage_variable_offset_matrix_stride
	(
		GLuint program, char const* name,
		GLint (&values)[2u]
	);
}
