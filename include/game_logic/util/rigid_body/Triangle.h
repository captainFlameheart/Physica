#pragma once
#include "glad_glfw.h"
#include "util/math/vector_2D.h"

namespace game_logic::util::rigid_body
{
	struct Triangle
	{
		GLint vertices[3];
		GLint body;
	};
}
