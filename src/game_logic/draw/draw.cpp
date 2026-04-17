#include "game_environment/environment.h"
#include "game_state/local_sizes/include.h"
#include "game_state/draw_primitive_types/include.h"
#include <iostream>

namespace game_logic::draw
{
	void draw_source_image(game_environment::Environment& environment)
	{
		glUseProgram(environment.state.shaders[static_cast<GLuint>(::game_state::shader_indices::draw::update_counts::Indices::update_counts)]);
		constexpr GLuint draw_entities_shader_count{ ::game_state::shader_indices::draw::entities::count };
		constexpr GLuint update_draw_count_local_size{ ::game_state::local_sizes::update_draw_counts_local_size };
		constexpr GLuint update_draw_count_work_group_count
		{
			(draw_entities_shader_count + update_draw_count_local_size - 1u) / update_draw_count_local_size
		};
		glDispatchCompute(update_draw_count_work_group_count, 1u, 1u);

		// IMPORTANT TODO: We might not need GL_SHADER_STORAGE_BARRIER_BIT!
		glMemoryBarrier(GL_COMMAND_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		for (GLuint draw_entities_shader_index{ ::game_state::shader_indices::draw::entities::base }; draw_entities_shader_index < ::game_state::shader_indices::draw::entities::end; ++draw_entities_shader_index)
		{
			glUseProgram(environment.state.shaders[draw_entities_shader_index]);
			GLuint index_in_draw_entities_shader_array{ draw_entities_shader_index - ::game_state::shader_indices::draw::entities::base };
			GLintptr command_offset
			{
				environment.state.layouts.commands.draw_arrays_commands_count_state.offset +
				index_in_draw_entities_shader_array * environment.state.layouts.commands.draw_arrays_commands_count_state.top_level_array_stride
			};
			glDrawArraysIndirect
			(
				::game_state::draw_primitive_types::entities_primitive_types[index_in_draw_entities_shader_array],
				reinterpret_cast<const void*>(static_cast<intptr_t>(command_offset))
			);
		}
	}

	void draw(game_environment::Environment& environment)
	{
		GLuint draw_framebuffer{ 0u };
		if (environment.state.holographic_radiance_cascades.enabled)
		{
			draw_framebuffer = environment.state.holographic_radiance_cascades.source_framebuffer;
		}
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, draw_framebuffer);

		draw_source_image(environment);

		if (environment.state.holographic_radiance_cascades.enabled)
		{
			if (environment.state.holographic_radiance_cascades.visible_source_layer == 0u)
			{
			}
			else
			{
				glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0u);
				glUseProgram(environment.state.shaders[static_cast<GLuint>(::game_state::shader_indices::draw::holographic_radiance_cascades::Indices::source_layer)]);
				glDrawArrays(GL_TRIANGLES, 0, 3u);
			}
		}

		++environment.state.draw_count;
	}
}
