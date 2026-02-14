#pragma once
#include "glad_glfw.h"

namespace game_logic::shader_util::query
{
	void query_shader_storage_variable_offset
	(
		GLuint program, char const* name,
		GLint (&values)[1u]
	);
}
