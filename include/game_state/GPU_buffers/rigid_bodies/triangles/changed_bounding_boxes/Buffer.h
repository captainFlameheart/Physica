#pragma once
#include <GLFW/glfw3.h>
#include "game_state/rigid_body/Triangle.h"

namespace game_state::GPU_buffers::rigid_bodies::triangles::changed_bounding_boxes
{
	struct Buffer
	{
		// TODO: Consider using std::430 layout to improve runtime performance
		GLuint buffer;
		GLint size;
		GLint size_offset;
		GLint boxes_stride;
		GLint boxes_index_offset;
		GLint boxes_min_x_offset;
		GLint boxes_min_y_offset;
		GLint boxes_max_x_offset;
		GLint boxes_max_y_offset;
	};
}
