#include "game_logic/tick/include.h"
#include "game_environment/environment.h"
#include "game_state/layouts/include.h"

namespace game_logic::tick
{
	void tick(game_environment::Environment& environment)
	{
		glUseProgram(environment.state.shaders[static_cast<GLuint>(::game_state::shader_indices::tick::process_entities::bodies::Indices::point_masses)]);
		constexpr GLuint process_point_masses_index{ 0u };
		GLintptr process_point_masses_command_offset
		{
			environment.state.layouts.fixed_data.dispatch_commands_work_group_count_x_state.offset +
			process_point_masses_index * environment.state.layouts.fixed_data.dispatch_commands_work_group_count_x_state.top_level_array_stride
		};
		glDispatchComputeIndirect(static_cast<GLintptr>(process_point_masses_command_offset));
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	}
}
