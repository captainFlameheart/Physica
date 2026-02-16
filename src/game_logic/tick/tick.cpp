#include "game_logic/tick/include.h"
#include "game_environment/environment.h"
#include "game_state/layouts/include.h"

namespace game_logic::tick
{
	void tick(game_environment::Environment& environment)
	{
		glUseProgram(environment.state.shaders[static_cast<GLuint>(::game_state::shader_indices::tick::update_counts::Indices::update_counts)]);
		constexpr GLuint tick_entities_shader_count{ ::game_state::shader_indices::tick::process_entities::count };
		constexpr GLuint update_tick_count_local_size{ ::game_state::local_sizes::update_tick_counts_local_size };
		constexpr GLuint update_tick_count_work_group_count
		{
			(tick_entities_shader_count + update_tick_count_local_size - 1u) / update_tick_count_local_size
		};
		glDispatchCompute(update_tick_count_work_group_count, 1u, 1u);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_COMMAND_BARRIER_BIT);

		glUseProgram(environment.state.shaders[static_cast<GLuint>(::game_state::shader_indices::tick::process_entities::bodies::Indices::point_masses)]);
		constexpr GLuint process_point_masses_index{ 0u };
		GLintptr process_point_masses_command_offset
		{
			environment.state.layouts.fixed_data.dispatch_commands_work_group_count_x_state.offset +
			process_point_masses_index * environment.state.layouts.fixed_data.dispatch_commands_work_group_count_x_state.top_level_array_stride
		};
		glDispatchComputeIndirect(static_cast<GLintptr>(process_point_masses_command_offset));
		
		// IMPORTANT TODO: Only have GL_UNIFORM_BARRIER_BIT right before drawing
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_UNIFORM_BARRIER_BIT);
	}
}
