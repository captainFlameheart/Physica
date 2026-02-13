#pragma once
#include "glad_glfw.h"

namespace game_state::shader_group
{
	struct Shader_Group
	{
		GLuint vertex_shader;
		GLuint fragment_shader;
		GLuint compute_shader;
	};
}
