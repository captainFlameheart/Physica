#pragma once
#include <GLFW/glfw3.h>

namespace game_state::GPU_buffers::fluid::bounding_boxes
{
	struct Buffer
	{
		GLuint buffer;
		GLint size;
		GLint boxes_offset;
		GLint boxes_stride;
	};
}
