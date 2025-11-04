#pragma once
#include <GLFW/glfw3.h>
#include "game_state/rigid_body/Triangle.h"

namespace game_state::GPU_buffers::rigid_bodies::triangles::contacts
{
	struct Buffer
	{
		// TODO: Consider using std::430 layout to improve runtime performance
		GLuint buffer;
		GLint size;
		GLint contacts_offset;
		GLint contacts_stride;

		GLuint current_persistent_contact_count;
	};
}
