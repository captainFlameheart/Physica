#include "game_logic/initialize/include.h"
#include "game_environment/environment.h"
#include <iostream>
#include "game_logic/OpenGL_capabilities/include.h"
#include "game_logic/binding_util/include.h"

namespace game_logic::initialize
{
	void initialize(game_environment::Environment& environment)
	{
		glCreateVertexArrays(1u, &environment.state.vertex_array);
		glBindVertexArray(environment.state.vertex_array);

		::game_logic::OpenGL_capabilities::query(environment.state.OpenGL_capabilities);
		::game_logic::OpenGL_capabilities::print(environment.state.OpenGL_capabilities);

		query_layouts::query_layouts(environment);
		compile_shaders::compile_shaders(environment);
		buffers::initialize(environment);

		::game_logic::binding_util::bind_GPU_only_buffer(environment, 0u);

		glUseProgram(environment.state.shaders[static_cast<GLuint>(::game_state::shader_indices::initialize::Indices::initialize)]);
		glDispatchCompute(1u, 1u, 1u);
		glMemoryBarrier(GL_ALL_BARRIER_BITS);
	}
}
