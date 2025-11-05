#pragma once
#include <GLFW/glfw3.h>
#include "game_state/rigid_body/Triangle.h"

namespace game_state::GPU_buffers::rigid_bodies::triangles::material_indices
{
	struct Buffer
	{
		GLuint buffer;
		GLint size;
		GLint material_indices_offset;
		GLint material_indices_stride;
	};
}
