#pragma once
#include <GLFW/glfw3.h>
#include "game_state/rigid_body/Triangle.h"
#include "bounding_boxes/include.h"
#include "vertices/include.h"
#include "contact_surfaces/include.h"
#include "contact_count/include.h"
#include "persistent_contact_count/include.h"
#include "changed_bounding_boxes/include.h"
#include "contacts/include.h"

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
		contact_surfaces::Buffer contact_surfaces;
		contact_count::Buffer contact_count;
		persistent_contact_count::Buffer persistent_contact_count;
		changed_bounding_boxes::Buffer changed_bounding_boxes;
		contacts::Buffer contacts;
	};
}
