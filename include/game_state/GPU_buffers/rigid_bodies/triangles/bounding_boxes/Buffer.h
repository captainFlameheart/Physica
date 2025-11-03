#pragma once
#include <GLFW/glfw3.h>
#include "game_state/rigid_body/Triangle.h"

namespace game_state::GPU_buffers::rigid_bodies::triangles::bounding_boxes
{
	struct Buffer
	{
		GLuint buffer;
		GLint size;
		GLint boxes_offset;
		GLint boxes_stride;
	};
}
