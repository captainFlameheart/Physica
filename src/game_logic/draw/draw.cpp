#include "game_environment/environment.h"
#include "game_state/local_sizes/include.h"
#include "game_state/draw_primitive_types/include.h"
#include "game_state/bindings/include.h"
#include "game_state/texture_units/include.h"
#include "game_logic/holographic_radiance_cascades/include.h"
#include <iostream>

namespace game_logic::draw
{
	void draw_source_image(game_environment::Environment& environment)
	{
		// VERY IMPORTANT TODO: Adapt drawBuffers based on which layers are actually used for each shader!
		// The unlit layer is often not used!!!

		for (GLuint draw_entities_shader_index{ ::game_state::shader_indices::draw::entities::base }; draw_entities_shader_index < ::game_state::shader_indices::draw::entities::skycircle_elements::base; ++draw_entities_shader_index)
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

		glUseProgram(environment.state.shaders[static_cast<GLuint>(::game_state::shader_indices::draw::cursor::Indices::cursor)]);
		glDrawArrays(GL_TRIANGLES, 0, 6u);
	}

	void draw_skycircle(game_environment::Environment& environment)
	{
		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		//glBlendFunc(GL_SRC1_COLOR, GL_ONE_MINUS_SRC1_COLOR);
		glBlendFuncSeparate(
			GL_SRC1_ALPHA, GL_ONE_MINUS_SRC1_ALPHA,  // RGB
			GL_SRC1_ALPHA, GL_ONE_MINUS_SRC1_ALPHA   // Alpha
		);

		for (GLuint draw_skycircle_elements_shader_index{ ::game_state::shader_indices::draw::entities::skycircle_elements::base }; draw_skycircle_elements_shader_index < ::game_state::shader_indices::draw::entities::skycircle_elements::end; ++draw_skycircle_elements_shader_index)
		{
			glUseProgram(environment.state.shaders[draw_skycircle_elements_shader_index]);
			GLuint index_in_draw_entities_shader_array{ draw_skycircle_elements_shader_index - ::game_state::shader_indices::draw::entities::base };
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

		glDisable(GL_BLEND);
	}

	void trace_rays
	(
		game_environment::Environment& environment,
		GLuint bidirection, GLuint orthogonal_bidirection, GLuint direction,
		GLuint trace_rays_cascade_count,
		GLuint probe_grid_length, GLuint orthogonal_probe_grid_length,
		GLuint& cascade
	)
	{
		for (cascade = 0u; cascade < trace_rays_cascade_count; ++cascade)
		{
			glUseProgram(environment.state.holographic_radiance_cascades.trace_rays_shaders[bidirection][direction][cascade]);

			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, environment.state.holographic_radiance_cascades.ray_framebuffers[cascade]);

			GLuint viewport_size[2u];
			GLuint cascade_power_of_two{ ::game_logic::holographic_radiance_cascades::compute_cascade_power_of_two(cascade) };
			::game_logic::holographic_radiance_cascades::compute_rays_viewport_size
			(
				viewport_size, probe_grid_length, cascade_power_of_two, cascade, orthogonal_probe_grid_length, bidirection, orthogonal_bidirection
			);
			glViewport(0, 0, viewport_size[0u], viewport_size[1u]);

			glDrawArrays(GL_TRIANGLES, 0, 3u);
		}
		//GLfloat clear_color[4u]{ 0.0f, 0.0f, 1.0f, 0.2f };
		//glClearTexImage(environment.state.holographic_radiance_cascades.source_texture, 0, GL_RGBA, GL_FLOAT, clear_color);
		//glClearTexImage(environment.state.holographic_radiance_cascades.ray_textures[0u], 0, GL_RGBA, GL_FLOAT, clear_color);
	}

	void merge_rays
	(
		game_environment::Environment& environment,
		GLuint bidirection, GLuint orthogonal_bidirection, GLuint direction,
		GLuint cascade_count, GLuint trace_rays_cascade_count,
		GLuint probe_grid_length, GLuint orthogonal_probe_grid_length,
		GLuint& cascade
	)
	{
		glUseProgram(environment.state.shaders[::game_state::shader_indices::draw::holographic_radiance_cascades::flatten_merge_rays[bidirection][direction]]);
		while (cascade < cascade_count)
		{
			GLuint configuration_offset
			{
				environment.state.holographic_radiance_cascades.configuration.offset_pairs[bidirection][direction].merge_rays +
				(cascade - trace_rays_cascade_count) * environment.state.holographic_radiance_cascades.configuration.merge_rays_stride
			};
			glBindBufferRange
			(
				GL_UNIFORM_BUFFER, ::game_state::bindings::uniform::private_input,
				environment.state.holographic_radiance_cascades.configuration.buffer,
				configuration_offset, environment.state.layouts.merge_rays_data.block_state.buffer_data_size
			);

			GLuint lower_cascade{ cascade - 1u };
			glBindTextureUnit(::game_state::texture_units::rays, environment.state.holographic_radiance_cascades.ray_textures[lower_cascade]);

			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, environment.state.holographic_radiance_cascades.ray_framebuffers[cascade]);

			GLuint viewport_size[2u];
			GLuint cascade_power_of_two{ ::game_logic::holographic_radiance_cascades::compute_cascade_power_of_two(cascade) };
			::game_logic::holographic_radiance_cascades::compute_rays_viewport_size
			(
				viewport_size, probe_grid_length, cascade_power_of_two, cascade, orthogonal_probe_grid_length, bidirection, orthogonal_bidirection
			);
			glViewport(0, 0, viewport_size[0u], viewport_size[1u]);

			glDrawArrays(GL_TRIANGLES, 0, 3u);

			++cascade;
		}
	}

	void compute_rays
	(
		game_environment::Environment& environment,
		GLuint bidirection, GLuint orthogonal_bidirection, GLuint direction,
		GLuint cascade_count, GLuint trace_rays_cascade_count,
		GLuint probe_grid_length, GLuint orthogonal_probe_grid_length,
		GLuint& cascade
	)
	{
		trace_rays(environment, bidirection, orthogonal_bidirection, direction, trace_rays_cascade_count, probe_grid_length, orthogonal_probe_grid_length, cascade);
		merge_rays(environment, bidirection, orthogonal_bidirection, direction, cascade_count, trace_rays_cascade_count, probe_grid_length, orthogonal_probe_grid_length, cascade);
	}

	void gather_fluence_from_skycircle
	(
		game_environment::Environment& environment,
		GLuint bidirection, GLuint orthogonal_bidirection, GLuint direction,
		GLuint probe_grid_length,
		GLuint& cascade
	)
	{
		glUseProgram(environment.state.shaders[::game_state::shader_indices::draw::holographic_radiance_cascades::flatten_gather_fluence_from_skycircle[bidirection][direction]]);

		glNamedFramebufferDrawBuffer(environment.state.holographic_radiance_cascades.angular_fluence_framebuffer, GL_COLOR_ATTACHMENT0);

		{
			GLuint viewport_size[2u];
			GLuint cascade_power_of_two{ ::game_logic::holographic_radiance_cascades::compute_cascade_power_of_two(cascade) };
			::game_logic::holographic_radiance_cascades::compute_gather_fluence_from_skycircle_viewport_size
			(
				viewport_size, probe_grid_length, cascade_power_of_two, cascade, bidirection, orthogonal_bidirection
			);
			glViewport(0, 0, viewport_size[0u], viewport_size[1u]);
		}

		glDrawArrays(GL_TRIANGLES, 0, 3u);
	}

	void merge_intermediate_fluence
	(
		game_environment::Environment& environment,
		GLuint bidirection, GLuint orthogonal_bidirection, GLuint direction,
		GLuint cascade_count,
		GLuint probe_grid_length, GLuint orthogonal_probe_grid_length,
		GLuint& cascade
	)
	{
		--cascade;
		while (cascade > 0u)
		{
			GLuint configuration_offset
			{
				environment.state.holographic_radiance_cascades.configuration.offset_pairs[bidirection][direction].merge_fluence +
				(cascade_count - 1u - cascade) * environment.state.holographic_radiance_cascades.configuration.merge_fluence_stride
			};
			glBindBufferRange
			(
				GL_UNIFORM_BUFFER, ::game_state::bindings::uniform::private_input,
				environment.state.holographic_radiance_cascades.configuration.buffer,
				configuration_offset, environment.state.layouts.merge_fluence_data.block_state.buffer_data_size
			);

			glBindTextureUnit(::game_state::texture_units::rays, environment.state.holographic_radiance_cascades.ray_textures[cascade]);

			GLuint output_layer{ ::game_logic::holographic_radiance_cascades::compute_fluence_layer(cascade_count, cascade) };
			glNamedFramebufferDrawBuffer(environment.state.holographic_radiance_cascades.angular_fluence_framebuffer, GL_COLOR_ATTACHMENT0 + output_layer);

			GLuint cascade_power_of_two{ ::game_logic::holographic_radiance_cascades::compute_cascade_power_of_two(cascade) };

			{
				GLuint inner_viewport_size[2u];
				::game_logic::holographic_radiance_cascades::compute_merge_fluence_inner_viewport_size
				(
					inner_viewport_size, probe_grid_length, cascade_power_of_two, cascade, orthogonal_probe_grid_length, bidirection, orthogonal_bidirection
				);
				glViewport(0, 0, inner_viewport_size[0u], inner_viewport_size[1u]);
				glDrawArrays(GL_TRIANGLES, 0, 3u);
			}

			{
				GLuint outer_viewport_start[2u];
				::game_logic::holographic_radiance_cascades::compute_merge_fluence_outer_viewport_start
				(
					outer_viewport_start, probe_grid_length, cascade_power_of_two, cascade, bidirection, orthogonal_bidirection
				);
				GLuint outer_viewport_size[2u];
				::game_logic::holographic_radiance_cascades::compute_merge_fluence_outer_viewport_size
				(
					outer_viewport_size, cascade_power_of_two, bidirection, orthogonal_bidirection
				);
				glViewport(outer_viewport_start[0u], outer_viewport_start[1u], outer_viewport_size[0u], outer_viewport_size[1u]);
				glDrawArrays(GL_TRIANGLES, 0, 3u);
			}

			--cascade;
		}
	}

	void merge_final_fluence
	(
		game_environment::Environment& environment,
		GLuint bidirection, GLuint orthogonal_bidirection, GLuint direction,
		GLuint cascade_count,
		GLuint probe_grid_length, GLuint orthogonal_probe_grid_length,
		bool& keep_fluence, GLuint& cascade
	)
	{
		GLuint configuration_offset
		{
			environment.state.holographic_radiance_cascades.configuration.offset_pairs[bidirection][direction].merge_fluence +
			(cascade_count - 1u) * environment.state.holographic_radiance_cascades.configuration.merge_fluence_stride
		};
		glBindBufferRange
		(
			GL_UNIFORM_BUFFER, ::game_state::bindings::uniform::private_input,
			environment.state.holographic_radiance_cascades.configuration.buffer,
			configuration_offset, environment.state.layouts.merge_fluence_data.block_state.buffer_data_size
		);

		glBindTextureUnit(::game_state::texture_units::rays, environment.state.holographic_radiance_cascades.ray_textures[0u]);

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, environment.state.holographic_radiance_cascades.fluence_framebuffer);

		GLuint cascade_power_of_two{ ::game_logic::holographic_radiance_cascades::compute_cascade_power_of_two(cascade) };

		glViewport(0, 0, environment.state.holographic_radiance_cascades.probe_grid_width, environment.state.holographic_radiance_cascades.probe_grid_height);

		if (keep_fluence)
		{
			glEnable(GL_BLEND);
		}

		glDrawArrays(GL_TRIANGLES, 0, 3u);

		if (keep_fluence)
		{
			glDisable(GL_BLEND);
		}
		keep_fluence = true;
	}

	void merge_fluence
	(
		game_environment::Environment& environment,
		GLuint bidirection, GLuint orthogonal_bidirection, GLuint direction,
		GLuint cascade_count,
		GLuint probe_grid_length, GLuint orthogonal_probe_grid_length,
		bool& keep_fluence, GLuint& cascade
	)
	{
		glUseProgram(environment.state.shaders[::game_state::shader_indices::draw::holographic_radiance_cascades::flatten_merge_fluence[bidirection][direction]]);
		merge_intermediate_fluence(environment, bidirection, orthogonal_bidirection, direction, cascade_count, probe_grid_length, orthogonal_probe_grid_length, cascade);
		merge_final_fluence(environment, bidirection, orthogonal_bidirection, direction, cascade_count, probe_grid_length, orthogonal_probe_grid_length, keep_fluence, cascade);
	}

	void gather_fluence
	(
		game_environment::Environment& environment,
		GLuint bidirection, GLuint orthogonal_bidirection, GLuint direction,
		GLuint cascade_count,
		GLuint probe_grid_length, GLuint orthogonal_probe_grid_length,
		bool& keep_fluence, GLuint& cascade
	)
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, environment.state.holographic_radiance_cascades.angular_fluence_framebuffer);
		gather_fluence_from_skycircle(environment, bidirection, orthogonal_bidirection, direction, probe_grid_length, cascade);
		merge_fluence(environment, bidirection, orthogonal_bidirection, direction, cascade_count, probe_grid_length, orthogonal_probe_grid_length, keep_fluence, cascade);
	}

	void generate_fluence_from_direction
	(
		game_environment::Environment& environment,
		GLuint bidirection, GLuint orthogonal_bidirection, GLuint direction,
		GLuint cascade_count, GLuint trace_rays_cascade_count,
		GLuint probe_grid_length, GLuint orthogonal_probe_grid_length,
		bool& keep_fluence
	)
	{
		GLuint cascade;
		compute_rays(environment, bidirection, orthogonal_bidirection, direction, cascade_count, trace_rays_cascade_count, probe_grid_length, orthogonal_probe_grid_length, cascade);
		gather_fluence(environment, bidirection, orthogonal_bidirection, direction, cascade_count, probe_grid_length, orthogonal_probe_grid_length, keep_fluence, cascade);
	}

	void generate_fluence(game_environment::Environment& environment)
	{
		// IMPORTANT TODO: Implement state cache to avoid redundant state changes.
		glDisable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ONE, GL_ONE);

		bool keep_fluence{ false };
		for (GLuint bidirection{ 0u }; bidirection < 2u; ++bidirection)
		{
			GLuint orthogonal_bidirection{ ::game_logic::holographic_radiance_cascades::compute_orthogonal_bidirection(bidirection) };
			GLuint trace_rays_cascade_count{ environment.state.holographic_radiance_cascades.trace_rays_cascade_counts[bidirection] };
			GLuint cascade_count{ environment.state.holographic_radiance_cascades.cascade_counts[bidirection] };
			GLuint probe_grid_length{ environment.state.holographic_radiance_cascades.probe_grid_size[bidirection] };
			GLuint orthogonal_probe_grid_length{ environment.state.holographic_radiance_cascades.probe_grid_size[orthogonal_bidirection] };

			for (GLuint direction{ 0u }; direction < 2u; ++direction)
			{
				if (environment.state.holographic_radiance_cascades.enabled_directions[bidirection][direction])
				{
					generate_fluence_from_direction(environment, bidirection, orthogonal_bidirection, direction, cascade_count, trace_rays_cascade_count, probe_grid_length, orthogonal_probe_grid_length, keep_fluence);
				}
			}
		}

		if (!keep_fluence)
		{
			GLfloat clear_color[4u]{ 0.0f, 0.0f, 0.0f, 0.0f };
			glClearTexImage(environment.state.holographic_radiance_cascades.fluence_texture, 0, GL_RGBA, GL_FLOAT, clear_color);
		}
	}

	void apply_fluence(game_environment::Environment& environment)
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0u);
		glViewport(0, 0, environment.state.glfw.framebuffer_width, environment.state.glfw.framebuffer_height);

		glUseProgram(environment.state.shaders[static_cast<GLuint>(::game_state::shader_indices::draw::holographic_radiance_cascades::Indices::apply_fluence)]);
		glDrawArrays(GL_TRIANGLES, 0, 3u);	// TODO: 6u when zoomed out.
	}

	void generate_and_apply_fluence(game_environment::Environment& environment)
	{
		generate_fluence(environment);
		apply_fluence(environment);
	}

	void draw(game_environment::Environment& environment)
	{
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_UNIFORM_BARRIER_BIT);

		//glUseProgram(environment.state.shaders[static_cast<GLuint>(::game_state::shader_indices::draw::highlight_hovered_objects::Indices::find_highlighted_hovered_leafs)]);
		//glDispatchCompute(1u, 1u, 1u);
		
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

		if (environment.state.holographic_radiance_cascades.enabled)
		{
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, environment.state.holographic_radiance_cascades.source_framebuffer);
			GLfloat clear_color[4u]{ 0.0f, 0.0f, 0.0f, 0.0f };
			glClearTexImage(environment.state.holographic_radiance_cascades.source_texture, 0, GL_RGBA, GL_FLOAT, clear_color);
		}
		else
		{
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0u);
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT);
		}
		draw_source_image(environment);

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, environment.state.holographic_radiance_cascades.skycircle_framebuffer);
		GLfloat clear_color[4u]{ 0.0f, 0.0f, 0.0f, 0.0f };
		glClearTexImage(environment.state.holographic_radiance_cascades.skycircle_texture, 0, GL_RGBA, GL_FLOAT, clear_color);
		glViewport(0, 0, environment.state.holographic_radiance_cascades.skycircle_length, 1u);
		draw_skycircle(environment);

		if (environment.state.holographic_radiance_cascades.enabled)
		{
			if (environment.state.holographic_radiance_cascades.visible_source_layer == 0u)
			{
				generate_and_apply_fluence(environment);
			}
			else
			{
				glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0u);
				glUseProgram(environment.state.shaders[static_cast<GLuint>(::game_state::shader_indices::draw::holographic_radiance_cascades::Indices::source_layer)]);
				glDrawArrays(GL_TRIANGLES, 0, 3u);
			}
			if (environment.state.holographic_radiance_cascades.draw_skycircle_ring)
			{
				glUseProgram(environment.state.shaders[static_cast<GLuint>(::game_state::shader_indices::draw::holographic_radiance_cascades::Indices::skycircle_ring)]);
				glDrawArrays(GL_TRIANGLES, 0, 6u);
			}
		}

		++environment.state.draw_count;
	}
}
