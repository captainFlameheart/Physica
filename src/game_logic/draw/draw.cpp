#include "game_environment/environment.h"
#include <iostream>

namespace game_logic::draw
{
	void draw(game_environment::Environment& environment)
	{
		//glUseProgram(environment.state.)

		//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		//glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(environment.state.shaders.state.draw.state.bodies.state.point_masses.state.draw);
		constexpr GLuint draw_point_masses_index{ 0u };
		GLintptr draw_point_masses_command_offset
		{
			environment.state.layouts.fixed_data.draw_arrays_commands_count_state.offset +
			draw_point_masses_index * environment.state.layouts.fixed_data.draw_arrays_commands_count_state.top_level_array_stride
		};
		/*glDrawArraysIndirect
		(
			GL_TRIANGLES,
			reinterpret_cast<const void*>(static_cast<intptr_t>(draw_point_masses_command_offset))
		);*/
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, environment.state.buffers.GPU_only.buffers[0u]);
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, environment.state.buffers.GPU_only.buffers[0u]);
		glDrawArrays(GL_TRIANGLES, 0, 6u);
	}
}
