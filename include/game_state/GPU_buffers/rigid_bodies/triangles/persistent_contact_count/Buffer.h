#pragma once
#include <GLFW/glfw3.h>
#include "game_state/rigid_body/Triangle.h"

namespace game_state::GPU_buffers::rigid_bodies::triangles::persistent_contact_count
{
	struct Buffer
	{
		GLint buffer;
		GLint size;
		GLint persistent_contact_count_offset;
	};
}
