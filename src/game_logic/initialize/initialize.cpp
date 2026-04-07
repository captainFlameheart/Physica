#include "game_logic/initialize/include.h"
#include "game_environment/environment.h"
#include <iostream>
#include "game_logic/OpenGL_capabilities/include.h"
#include "game_logic/binding_util/include.h"

namespace game_logic::initialize
{
	void initialize(game_environment::Environment& environment)
	{
		environment.state.is_debugging = false;

		environment.state.tick_paused = false;

		glCreateVertexArrays(1u, &environment.state.vertex_array);
		glBindVertexArray(environment.state.vertex_array);

		environment.state.tick_count = 0u;
		environment.state.draw_count = 0u;

		// TODO: Initialize viewport and glfw data

		::game_logic::OpenGL_capabilities::query(environment.state.OpenGL_capabilities);
		::game_logic::OpenGL_capabilities::print(environment.state.OpenGL_capabilities);

		query_layouts::query_layouts(environment);
		compile_shaders::compile_shaders(environment);
		buffers::initialize(environment);

		::game_logic::binding_util::bind_GPU_only_buffer(environment, 0u);

		glUseProgram(environment.state.shaders[static_cast<GLuint>(::game_state::shader_indices::initialize::Indices::initialize)]);
		glDispatchCompute(1u, 1u, 1u);
		glMemoryBarrier(GL_ALL_BARRIER_BITS);

		{
			glUseProgram(environment.state.shaders[static_cast<GLuint>(::game_state::shader_indices::tick::update_counts::Indices::update_counts)]);
			constexpr GLuint tick_entities_shader_count{ ::game_state::shader_indices::tick::process_entities::count - ::game_state::shader_indices::tick::process_entities::pre_constraint_spawners::count };
			constexpr GLuint update_tick_count_local_size{ ::game_state::local_sizes::update_tick_counts_local_size };
			constexpr GLuint update_tick_count_work_group_count
			{
				(tick_entities_shader_count + update_tick_count_local_size - 1u) / update_tick_count_local_size
			};
			glDispatchCompute(update_tick_count_work_group_count, 1u, 1u);

			// MUST TODO: Update read counts in separate shader!!!

			glMemoryBarrier(GL_ALL_BARRIER_BITS);
		}
	}
}
