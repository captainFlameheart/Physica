#include "game_logic/tick/include.h"
#include "game_environment/environment.h"
#include "game_state/layouts/include.h"

namespace game_logic::tick
{
	void tick_bodies(game_environment::Environment& environment)
	{
		for (GLuint tick_bodies_shader_index{ ::game_state::shader_indices::tick::process_entities::bodies::base }; tick_bodies_shader_index < ::game_state::shader_indices::tick::process_entities::bodies::end; ++tick_bodies_shader_index)
		{
			glUseProgram(environment.state.shaders[tick_bodies_shader_index]);
			GLuint index_in_tick_entities_shader_array{ tick_bodies_shader_index - ::game_state::shader_indices::tick::process_entities::base };
			GLintptr command_offset
			{
				environment.state.layouts.commands.dispatch_commands_work_group_count_x_state.offset +
				index_in_tick_entities_shader_array * environment.state.layouts.commands.dispatch_commands_work_group_count_x_state.top_level_array_stride
			};
			glDispatchComputeIndirect(static_cast<GLintptr>(command_offset));
		}
	}

	GLsync send_bounding_volume_hierarchy_metadata_to_CPU(game_environment::Environment& environment)
	{
		glUseProgram(environment.state.shaders[static_cast<GLuint>(::game_state::shader_indices::tick::process_entities::bounding_volume_hierarchy::send_to_CPU::Indices::send_to_CPU)]);
		glDispatchCompute(1u, 1u, 1u);
		glMemoryBarrier(GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT);
		return glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0u);
	}

	void tick_bounding_volume_hierarchy_leafs(game_environment::Environment& environment)
	{
		// MUST TODO: Set work group counts!
		for (GLuint tick_bounding_volume_hierarchy_leafs_shader_index{ ::game_state::shader_indices::tick::process_entities::bounding_volume_hierarchy::leafs::base }; tick_bounding_volume_hierarchy_leafs_shader_index < ::game_state::shader_indices::tick::process_entities::bounding_volume_hierarchy::leafs::end; ++tick_bounding_volume_hierarchy_leafs_shader_index)
		{
			glUseProgram(environment.state.shaders[tick_bounding_volume_hierarchy_leafs_shader_index]);
			GLuint index_in_tick_entities_shader_array{ tick_bounding_volume_hierarchy_leafs_shader_index - ::game_state::shader_indices::tick::process_entities::base };
			GLintptr command_offset
			{
				environment.state.layouts.commands.dispatch_commands_work_group_count_x_state.offset +
				index_in_tick_entities_shader_array * environment.state.layouts.commands.dispatch_commands_work_group_count_x_state.top_level_array_stride
			};
			glDispatchComputeIndirect(static_cast<GLintptr>(command_offset));
		}
	}

	void find_inner_bounding_box_height_changes(game_environment::Environment& environment)
	{
		glUseProgram(environment.state.shaders[static_cast<GLuint>(::game_state::shader_indices::tick::process_entities::bounding_volume_hierarchy::initialize_inner_bounding_box_traversal::Indices::find_height_changes)]);
		constexpr GLuint index_in_tick_entities_shader_array{ static_cast<GLuint>(::game_state::shader_indices::tick::process_entities::bounding_volume_hierarchy::initialize_inner_bounding_box_traversal::Indices::find_height_changes) - ::game_state::shader_indices::tick::process_entities::base };
		GLintptr command_offset
		{
			environment.state.layouts.commands.dispatch_commands_work_group_count_x_state.offset +
			index_in_tick_entities_shader_array * environment.state.layouts.commands.dispatch_commands_work_group_count_x_state.top_level_array_stride
		};
		glDispatchComputeIndirect(command_offset);
	}

	void tick_skycircle_elements(game_environment::Environment& environment)
	{
		for (GLuint tick_skycircle_elements_shader_index{ ::game_state::shader_indices::tick::process_entities::skycircle_elements::base }; tick_skycircle_elements_shader_index < ::game_state::shader_indices::tick::process_entities::skycircle_elements::end; ++tick_skycircle_elements_shader_index)
		{
			glUseProgram(environment.state.shaders[tick_skycircle_elements_shader_index]);
			GLuint index_in_tick_entities_shader_array{ tick_skycircle_elements_shader_index - ::game_state::shader_indices::tick::process_entities::base };
			GLintptr command_offset
			{
				environment.state.layouts.commands.dispatch_commands_work_group_count_x_state.offset +
				index_in_tick_entities_shader_array * environment.state.layouts.commands.dispatch_commands_work_group_count_x_state.top_level_array_stride
			};
			glDispatchComputeIndirect(static_cast<GLintptr>(command_offset));
		}
	}

	void wait_for_fence
	(
		GLsync fence,
		std::string error_message,
		std::string timeout_message = "",
		bool show_timeout_message = false
	)
	{
		GLenum fence_status{ glClientWaitSync(fence, GL_SYNC_FLUSH_COMMANDS_BIT, 0u) };
		while (fence_status != GL_ALREADY_SIGNALED && fence_status != GL_CONDITION_SATISFIED)
		{
			if (fence_status == GL_WAIT_FAILED)
			{
				std::cerr << error_message << std::endl;
			}
			if (show_timeout_message && fence_status == GL_TIMEOUT_EXPIRED)
			{
				std::cout << timeout_message << std::endl;
			}
			fence_status = glClientWaitSync(fence, 0u, 0u);
		}
		glDeleteSync(fence);
	}

	GLuint get_bounding_volume_hierarchy_height(game_environment::Environment& environment)
	{
		GLuint bounding_volume_hierarchy_height;
		std::memcpy
		(
			&bounding_volume_hierarchy_height,
			environment.state.buffers.bounding_volume_hierarchy.mapping + environment.state.layouts.bounding_volume_hierarchy.height_state.offset,
			sizeof(GLuint)
		);
		return bounding_volume_hierarchy_height;
	}

	void migrate_inner_bounding_boxes(game_environment::Environment& environment)
	{
		glUseProgram(environment.state.shaders[static_cast<GLuint>(::game_state::shader_indices::tick::process_entities::bounding_volume_hierarchy::initialize_inner_bounding_box_traversal::Indices::migrate_inner_bounding_boxes)]);
		glDispatchCompute(1u, 1u, 1u);
	}

	void initialize_inner_bounding_box_traversal(game_environment::Environment& environment, GLuint bounding_volume_hierarchy_height)
	{
		{
			glUseProgram(environment.state.shaders[static_cast<GLuint>(::game_state::shader_indices::tick::process_entities::bounding_volume_hierarchy::initialize_inner_bounding_box_traversal::Indices::initialize_inner_bounding_box_traversal)]);
			glDispatchCompute(1u, 1u, 1u);
		}
		{
			glUseProgram(environment.state.shaders[static_cast<GLuint>(::game_state::shader_indices::tick::process_entities::bounding_volume_hierarchy::initialize_inner_bounding_box_traversal::Indices::set_commands)]);
			GLuint thread_count{ bounding_volume_hierarchy_height };
			constexpr GLuint local_size{ ::game_state::local_sizes::process_entities_local_sizes[static_cast<GLuint>(::game_state::shader_indices::tick::process_entities::bounding_volume_hierarchy::initialize_inner_bounding_box_traversal::Indices::set_commands) - static_cast<GLuint>(::game_state::shader_indices::tick::process_entities::base)] };
			glDispatchCompute((thread_count + local_size - 1u) / local_size, 1u, 1u);
		}
	}

	void tick_deepest_inner_bounding_boxes(game_environment::Environment& environment)
	{
		glUseProgram(environment.state.shaders[static_cast<GLuint>(::game_state::shader_indices::tick::process_entities::bounding_volume_hierarchy::inner_bounding_boxes::Indices::inner_bounding_boxes)]);
		GLintptr command_offset
		{
			environment.state.layouts.commands.remaining_dispatch_commands_work_group_count_x_state.offset
		};
		glDispatchComputeIndirect(static_cast<GLintptr>(command_offset));
	}

	void tick_remaining_inner_bounding_boxes(game_environment::Environment& environment, GLuint bounding_volume_hierarchy_height)
	{
		for (GLuint height_level{ 1u }; height_level <= bounding_volume_hierarchy_height; ++height_level) {
			glUseProgram(environment.state.shaders[static_cast<GLuint>(::game_state::shader_indices::tick::process_entities::bounding_volume_hierarchy::inner_bounding_boxes::Indices::increment_height_level)]);
			glDispatchCompute(1u, 1u, 1u);

			glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

			glUseProgram(environment.state.shaders[static_cast<GLuint>(::game_state::shader_indices::tick::process_entities::bounding_volume_hierarchy::inner_bounding_boxes::Indices::inner_bounding_boxes)]);
			GLintptr command_offset
			{
				environment.state.layouts.commands.remaining_dispatch_commands_work_group_count_x_state.offset +
				height_level * environment.state.layouts.commands.remaining_dispatch_commands_work_group_count_x_state.top_level_array_stride
			};
			glDispatchComputeIndirect(static_cast<GLintptr>(command_offset));

			glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
		}
	}

	void tick_inner_bounding_boxes(game_environment::Environment& environment, GLuint bounding_volume_hierarchy_height)
	{
		tick_deepest_inner_bounding_boxes(environment);

		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

		tick_remaining_inner_bounding_boxes(environment, bounding_volume_hierarchy_height);
	}

	void tick(game_environment::Environment& environment)
	{
		if (environment.state.tick_count % 1u == 0u)	// TODO: Make into proper feature
		{
			tick_bodies(environment);

			glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

			GLsync bounding_volume_hierarchy_fence{ send_bounding_volume_hierarchy_metadata_to_CPU(environment) };

			tick_bounding_volume_hierarchy_leafs(environment);
			find_inner_bounding_box_height_changes(environment);
			tick_skycircle_elements(environment);

			glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);	// Dependency in mind: find_inner_bounding_box_height_changes -> migrate_inner_bounding_boxes

			wait_for_fence(bounding_volume_hierarchy_fence, "Failed to wait on bounding volume hierarchy fence!", "Had to wait on bounding volume hierarchy fence!", false);
			GLuint bounding_volume_hierarchy_height{ get_bounding_volume_hierarchy_height(environment) };

			migrate_inner_bounding_boxes(environment);	// TODO: Can currently be done before waiting for BVH height, but maybe not when properly parallelized.

			glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

			initialize_inner_bounding_box_traversal(environment, bounding_volume_hierarchy_height);
			
			glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_COMMAND_BARRIER_BIT);

			tick_inner_bounding_boxes(environment, bounding_volume_hierarchy_height);

			glUseProgram
			(
				environment.state.shaders[static_cast<GLuint>(::game_state::shader_indices::tick::process_entities::pre_constraint_spawners::commit_counts::Indices::commit_counts)]
			);
			constexpr GLuint constraint_spawner_type_count{ ::game_state::entity_type_indices::constraint_spawners::count };
			constexpr GLuint commit_constraint_spawner_counts_local_size
			{
				::game_state::local_sizes::process_entities_local_sizes
				[
					static_cast<GLuint>(::game_state::shader_indices::tick::process_entities::pre_constraint_spawners::commit_counts::Indices::commit_counts)
					- static_cast<GLuint>(::game_state::shader_indices::tick::process_entities::base)
				]
			};
			constexpr GLuint commit_constraint_spawner_counts_work_group_count
			{
				(constraint_spawner_type_count + commit_constraint_spawner_counts_local_size - 1u) / commit_constraint_spawner_counts_local_size
			};
			glDispatchCompute(commit_constraint_spawner_counts_work_group_count, 1u, 1u);

			glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_COMMAND_BARRIER_BIT);

			for (GLuint clear_constraint_spawner_deaths_shader_index{::game_state::shader_indices::tick::process_entities::pre_constraint_spawners::clear_deaths::base}; clear_constraint_spawner_deaths_shader_index < ::game_state::shader_indices::tick::process_entities::pre_constraint_spawners::clear_deaths::end; ++clear_constraint_spawner_deaths_shader_index)
			{
				glUseProgram(environment.state.shaders[clear_constraint_spawner_deaths_shader_index]);
				// TODO: We should exclude commit-count shaders.
				GLuint index_in_tick_entities_shader_array{ clear_constraint_spawner_deaths_shader_index - ::game_state::shader_indices::tick::process_entities::base };
				GLintptr command_offset
				{
					environment.state.layouts.commands.dispatch_commands_work_group_count_x_state.offset +
					index_in_tick_entities_shader_array * environment.state.layouts.commands.dispatch_commands_work_group_count_x_state.top_level_array_stride
				};
				glDispatchComputeIndirect(command_offset);

				if (environment.state.debug_flag == 1u)
				{
					::game_logic::debug::print_fixed_data(environment);
				}
			}

			for (GLuint perform_constraint_spawner_compaction_shader_index{::game_state::shader_indices::tick::process_entities::pre_constraint_spawners::perform_compaction::base}; perform_constraint_spawner_compaction_shader_index < ::game_state::shader_indices::tick::process_entities::pre_constraint_spawners::perform_compaction::end; ++perform_constraint_spawner_compaction_shader_index)
			{
				glUseProgram(environment.state.shaders[perform_constraint_spawner_compaction_shader_index]);
				// TODO: We should exclude commit-count shaders.
				GLuint index_in_tick_entities_shader_array{ perform_constraint_spawner_compaction_shader_index - ::game_state::shader_indices::tick::process_entities::base };
				GLintptr command_offset
				{
					environment.state.layouts.commands.dispatch_commands_work_group_count_x_state.offset +
					index_in_tick_entities_shader_array * environment.state.layouts.commands.dispatch_commands_work_group_count_x_state.top_level_array_stride
				};
				glDispatchComputeIndirect(command_offset);

				if (environment.state.debug_flag == 1u)
				{
					::game_logic::debug::print_fixed_data(environment);	// THE PROBLEM IS HERE!!! We likely get stuck in an infinite loop.
				}
			}

			glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

			for (GLuint tick_constraint_spawners_shader_index{::game_state::shader_indices::tick::process_entities::constraint_spawners::base}; tick_constraint_spawners_shader_index < ::game_state::shader_indices::tick::process_entities::constraint_spawners::end; ++tick_constraint_spawners_shader_index)
			{
				glUseProgram(environment.state.shaders[tick_constraint_spawners_shader_index]);
				GLuint index_in_tick_entities_shader_array{ tick_constraint_spawners_shader_index - ::game_state::shader_indices::tick::process_entities::base };
				GLintptr command_offset
				{
					environment.state.layouts.commands.dispatch_commands_work_group_count_x_state.offset +
					index_in_tick_entities_shader_array * environment.state.layouts.commands.dispatch_commands_work_group_count_x_state.top_level_array_stride
				};
				glDispatchComputeIndirect(command_offset);

				if (environment.state.debug_flag == 1u)
				{
					::game_logic::debug::print_fixed_data(environment);
				}
			}

			if (environment.state.is_debugging)
			{
				debug::print_constraint_spawner_compaction_plan(environment);
			}

			glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

			// TODO: REMOVE.
			// MUST TODO: Update counts after constraint tick as well!
			glUseProgram(environment.state.shaders[static_cast<GLuint>(::game_state::shader_indices::tick::update_counts::Indices::update_counts)]);
			constexpr GLuint tick_entities_shader_count{ ::game_state::shader_indices::tick::process_entities::count - ::game_state::shader_indices::tick::process_entities::pre_constraint_spawners::count };
			constexpr GLuint update_tick_count_local_size{ ::game_state::local_sizes::update_tick_counts_local_size };
			constexpr GLuint update_tick_count_work_group_count
			{
				(tick_entities_shader_count + update_tick_count_local_size - 1u) / update_tick_count_local_size
			};
			glDispatchCompute(update_tick_count_work_group_count, 1u, 1u);

			// MUST TODO: Update read counts in separate shader!!!

			glUseProgram(environment.state.shaders[static_cast<GLuint>(::game_state::shader_indices::tick::update_counts::Indices::on_update_counts)]);
			glDispatchCompute(1u, 1u, 1u);

			glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_COMMAND_BARRIER_BIT);

			glUseProgram
			(
				environment.state.shaders[static_cast<GLuint>(::game_state::shader_indices::tick::process_entities::pre_constraints::commit_counts::Indices::commit_counts)]
			);
			constexpr GLuint constraint_type_count{ ::game_state::entity_type_indices::constraints::count };
			constexpr GLuint commit_constraint_counts_local_size
			{
				::game_state::local_sizes::process_entities_local_sizes
				[
					static_cast<GLuint>(::game_state::shader_indices::tick::process_entities::pre_constraints::commit_counts::Indices::commit_counts)
					- static_cast<GLuint>(::game_state::shader_indices::tick::process_entities::base)
				]
			};
			constexpr GLuint commit_constraint_counts_work_group_count
			{
				(constraint_spawner_type_count + commit_constraint_counts_local_size - 1u) / commit_constraint_counts_local_size
			};
			glDispatchCompute(commit_constraint_counts_work_group_count, 1u, 1u);

			glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_COMMAND_BARRIER_BIT);

			for (GLuint clear_constraint_deaths_shader_index{ ::game_state::shader_indices::tick::process_entities::pre_constraints::clear_deaths::base }; clear_constraint_deaths_shader_index < ::game_state::shader_indices::tick::process_entities::pre_constraints::clear_deaths::end; ++clear_constraint_deaths_shader_index)
			{
				// TODO: REMOVE.
				if (clear_constraint_deaths_shader_index != static_cast<GLuint>(::game_state::shader_indices::tick::process_entities::pre_constraints::clear_deaths::Indices::clear_rigid_body_circle_contact_constraint_deaths))
				{
					continue;
				}

				glUseProgram(environment.state.shaders[clear_constraint_deaths_shader_index]);
				// TODO: We should exclude commit-count shaders.
				GLuint index_in_tick_entities_shader_array{ clear_constraint_deaths_shader_index - ::game_state::shader_indices::tick::process_entities::base };
				GLintptr command_offset
				{
					environment.state.layouts.commands.dispatch_commands_work_group_count_x_state.offset +
					index_in_tick_entities_shader_array * environment.state.layouts.commands.dispatch_commands_work_group_count_x_state.top_level_array_stride
				};
				glDispatchComputeIndirect(command_offset);
			}

			for (GLuint perform_constraint_compaction_shader_index{::game_state::shader_indices::tick::process_entities::pre_constraints::perform_compaction::base}; perform_constraint_compaction_shader_index < ::game_state::shader_indices::tick::process_entities::pre_constraints::perform_compaction::end; ++perform_constraint_compaction_shader_index)
			{
				// TODO: REMOVE.
				if (perform_constraint_compaction_shader_index != static_cast<GLuint>(::game_state::shader_indices::tick::process_entities::pre_constraints::perform_compaction::Indices::perform_rigid_body_circle_contact_constraint_compaction))
				{
					continue;
				}

				glUseProgram(environment.state.shaders[perform_constraint_compaction_shader_index]);
				// TODO: We should exclude commit-count shaders.
				GLuint index_in_tick_entities_shader_array{ perform_constraint_compaction_shader_index - ::game_state::shader_indices::tick::process_entities::base };
				GLintptr command_offset
				{
					environment.state.layouts.commands.dispatch_commands_work_group_count_x_state.offset +
					index_in_tick_entities_shader_array * environment.state.layouts.commands.dispatch_commands_work_group_count_x_state.top_level_array_stride
				};
				glDispatchComputeIndirect(command_offset);
			}

			glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

			glUseProgram(environment.state.shaders[static_cast<GLuint>(::game_state::shader_indices::tick::process_entities::on_tick_constraints::Indices::on_tick_constraints)]);
			glDispatchCompute(1u, 1u, 1u);

			for (GLuint tick_constraints_shader_index{ ::game_state::shader_indices::tick::process_entities::constraints::base }; tick_constraints_shader_index < ::game_state::shader_indices::tick::process_entities::constraints::end; ++tick_constraints_shader_index)
			{
				/*if (tick_constraints_shader_index != ::game_state::shader_indices::tick::process_entities::constraints::Indices::point_mass_distance_constraints)
				{
					return;
				}*/

				glUseProgram(environment.state.shaders[tick_constraints_shader_index]);
				GLuint index_in_tick_entities_shader_array{ tick_constraints_shader_index - ::game_state::shader_indices::tick::process_entities::base };
				GLintptr command_offset
				{
					environment.state.layouts.commands.dispatch_commands_work_group_count_x_state.offset +
					index_in_tick_entities_shader_array * environment.state.layouts.commands.dispatch_commands_work_group_count_x_state.top_level_array_stride
				};
				glDispatchComputeIndirect(command_offset);
			}

			// IMPORTANT TODO: Only have GL_UNIFORM_BARRIER_BIT right before drawing
			glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_UNIFORM_BARRIER_BIT);
		}

		++environment.state.tick_count;
	}
}
