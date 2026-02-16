#include "game_environment/environment.h"
#include <iostream>

namespace game_logic::draw
{
	void draw(game_environment::Environment& environment)
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(environment.state.shaders[static_cast<GLuint>(::game_state::shader_indices::draw::entities::bodies::Indices::point_masses)]);
		constexpr GLuint draw_point_masses_index{ 0u };
		GLintptr draw_point_masses_command_offset
		{
			environment.state.layouts.fixed_data.draw_arrays_commands_count_state.offset +
			draw_point_masses_index * environment.state.layouts.fixed_data.draw_arrays_commands_count_state.top_level_array_stride
		};
		glDrawArraysIndirect
		(
			GL_TRIANGLES,
			reinterpret_cast<const void*>(static_cast<intptr_t>(draw_point_masses_command_offset))
		);
	}
}
