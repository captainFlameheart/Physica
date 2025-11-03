#pragma once
#include <GLFW/glfw3.h>
#include "game_state/rigid_body/Triangle.h"

namespace game_state::GPU_buffers::rigid_bodies::triangles::vertices
{
	struct Buffer
	{
		GLuint buffer;
		GLint size;
		GLint vertices_offset;
		GLint vertices_stride;

		GLfloat(*values)[2u];
	};
}
