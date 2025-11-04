#pragma once
#include <GLFW/glfw3.h>

namespace game_state::GPU_buffers::fluid::changed_bounding_boxes
{
	struct Buffer
	{
		GLuint buffer;
		GLint size;
		GLint size_offset;
		GLint boxes_stride;
		GLint boxes_index_offset;
		GLint boxes_min_x_offset;
		GLint boxes_min_y_offset;
		GLint boxes_max_x_offset;
		GLint boxes_max_y_offset;

		unsigned char* mapping;
	};
}
