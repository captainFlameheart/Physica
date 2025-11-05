#pragma once
#include <GLFW/glfw3.h>
#include "game_state/rigid_body/Triangle.h"

namespace game_state::GPU_buffers::rigid_bodies::triangles::materials
{
	struct Buffer
	{
		GLuint buffer;
		GLint size;
		GLint materials_albedo_offset;
		GLint materials_emission_offset;
		GLint materials_absorption_offset;
		GLint materials_scattering_offset;

		GLint materials_offset;
		GLint materials_stride;
	};
}
