#pragma once
#include <GLFW/glfw3.h>
#include "game_state/rigid_body/Triangle.h"
#include "bounding_boxes/include.h"
#include "vertices/include.h"

namespace game_state::GPU_buffers::rigid_bodies::triangles
{
	struct Buffer
	{
		GLuint buffer;
		GLint size;
		GLint triangles_offset;
		GLint triangles_stride;

		GLuint current_count;
		rigid_body::Triangle* values;

		vertices::Buffer vertices;
		bounding_boxes::Buffer bounding_boxes;
	};
}
