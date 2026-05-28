#include "game_logic/initialize/include.h"
#include "game_environment/environment.h"
#include <iostream>
#include "game_logic/OpenGL_capabilities/include.h"
#include "game_logic/binding_util/include.h"

namespace game_logic::initialize
{
	void initialize(game_environment::Environment& environment)
	{
		environment.state.is_profiling_inner_bounding_box_migrations = false;
		environment.state.is_profiling_bounding_volume_hierarchy_rotations = false;
		environment.state.is_profiling_contacts = false;

		environment.state.debug_flag = 0u;

		::game_logic::profiling::initialize_state(environment);

		glfw::initialize(environment);

		glCreateVertexArrays(1u, &environment.state.vertex_array);
		glBindVertexArray(environment.state.vertex_array);

		environment.state.is_debugging = false;
		environment.state.tick_paused = false;

		environment.state.tick_count = 0u;
		environment.state.draw_count = 0u;

		::game_logic::OpenGL_capabilities::query(environment.state.OpenGL_capabilities);
		::game_logic::OpenGL_capabilities::print(environment.state.OpenGL_capabilities);

		query_layouts::query_layouts(environment);
		holographic_radiance_cascades::initialize(environment);
		compile_shaders::compile_shaders(environment);
		buffers::initialize(environment);

		::game_logic::binding_util::bind_GPU_only_buffer(environment, 0u);

		glUseProgram(environment.state.shaders[static_cast<GLuint>(::game_state::shader_indices::initialize::Indices::initialize)]);
		glDispatchCompute(1u, 1u, 1u);
		glMemoryBarrier(GL_ALL_BARRIER_BITS);

		::game_logic::tasks::initialize_contacts(environment);

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

		::game_logic::GLFW::toggle_fullscreen(environment);
		//::game_logic::profiling::enable_timing(environment, static_cast<GLuint>(::game_state::profiling::Timestamp_Type::contacts));
		//::game_logic::debug::print_fixed_data(environment);
	}
}
