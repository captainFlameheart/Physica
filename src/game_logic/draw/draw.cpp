#include "game_environment/environment.h"
#include "game_state/local_sizes/include.h"
#include "game_state/draw_primitive_types/include.h"
#include <iostream>

namespace game_logic::draw
{
	void draw(game_environment::Environment& environment)
	{
		glUseProgram(environment.state.shaders[static_cast<GLuint>(::game_state::shader_indices::draw::update_counts::Indices::update_counts)]);
		constexpr GLuint draw_entities_shader_count{ ::game_state::shader_indices::draw::entities::count };
		constexpr GLuint update_draw_count_local_size{ ::game_state::local_sizes::update_draw_counts_local_size };
		constexpr GLuint update_draw_count_work_group_count
		{
			(draw_entities_shader_count + update_draw_count_local_size - 1u) / update_draw_count_local_size
		};
		glDispatchCompute(update_draw_count_work_group_count, 1u, 1u);

		glMemoryBarrier(GL_COMMAND_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);	// Assuming we don't need to read the vertex counts

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		for (GLuint draw_entities_shader_index{ ::game_state::shader_indices::draw::entities::base }; draw_entities_shader_index < ::game_state::shader_indices::draw::entities::end; ++draw_entities_shader_index)
		{
			if (draw_entities_shader_index != ::game_state::shader_indices::draw::entities::base)
			{
				//break;	// TODO: Remove
			}

			glUseProgram(environment.state.shaders[draw_entities_shader_index]);
			GLuint index_in_draw_entities_shader_array{ draw_entities_shader_index - ::game_state::shader_indices::draw::entities::base };
			GLintptr command_offset
			{
				environment.state.layouts.fixed_data.draw_arrays_commands_count_state.offset +
				index_in_draw_entities_shader_array * environment.state.layouts.fixed_data.draw_arrays_commands_count_state.top_level_array_stride
			};
			glDrawArraysIndirect
			(
				::game_state::draw_primitive_types::entities_primitive_types[index_in_draw_entities_shader_array],
				reinterpret_cast<const void*>(static_cast<intptr_t>(command_offset))
			);
		}

		++environment.state.draw_count;
	}
}
