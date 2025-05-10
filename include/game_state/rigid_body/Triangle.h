#pragma once
#include "glad_glfw.h"
#include "util/math/vector_2D.h"

namespace game_state::rigid_body
{
	struct Triangle
	{
		GLuint vertices[3];
		GLuint body;
	};
}
