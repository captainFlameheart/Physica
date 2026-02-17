#pragma once
#include "glad_glfw.h"
#include "OpenGL_capabilities/include.h"
#include "layouts/include.h"
#include "buffers/include.h"
#include "shader_indices/include.h"
#include "glfw/include.h"

namespace game_state
{
	struct Game
	{
		GLuint vertex_array;	// Required for drawing anything at all

		GLuint tick_count;
		GLuint draw_count;

		glfw::State glfw;

		OpenGL_capabilities::State OpenGL_capabilities;
		GLuint shaders[shader_indices::count];
		layouts::State layouts;
		buffers::State buffers;
	};
}
