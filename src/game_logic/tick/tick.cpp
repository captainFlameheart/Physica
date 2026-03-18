#include "game_logic/tick/include.h"
#include "game_environment/environment.h"
#include "game_state/layouts/include.h"

namespace game_logic::tick
{
	void tick(game_environment::Environment& environment)
	{
		if (environment.state.tick_count % 1u == 0u)	// TODO: Make into proper feature
		{
			if (environment.state.is_debugging)
			{
				::game_logic::debug::print_fixed_data(environment);
			}

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

			glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

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
				//glDispatchComputeIndirect(command_offset);
			}

			/*for (GLuint plan_constraint_spawner_compaction_shader_index{::game_state::shader_indices::tick::process_entities::pre_constraint_spawners::plan_compaction::base}; plan_constraint_spawner_compaction_shader_index < ::game_state::shader_indices::tick::process_entities::pre_constraint_spawners::plan_compaction::end; ++plan_constraint_spawner_compaction_shader_index)
			{
				glUseProgram(environment.state.shaders[plan_constraint_spawner_compaction_shader_index]);
				// TODO: We should exclude commit-count shaders.
				GLuint index_in_tick_entities_shader_array{ plan_constraint_spawner_compaction_shader_index - ::game_state::shader_indices::tick::process_entities::base };
				GLintptr command_offset
				{
					environment.state.layouts.commands.dispatch_commands_work_group_count_x_state.offset +
					index_in_tick_entities_shader_array * environment.state.layouts.commands.dispatch_commands_work_group_count_x_state.top_level_array_stride
				};
				glDispatchComputeIndirect(command_offset);
			}

			glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);*/

			/*if (environment.state.is_debugging)
			{
				debug::print_constraint_spawner_compaction_plan(environment);
			}*/

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
				//glDispatchComputeIndirect(command_offset);
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
				//glDispatchComputeIndirect(command_offset);
			}

			/*for (GLuint plan_constraint_compaction_shader_index{::game_state::shader_indices::tick::process_entities::pre_constraints::plan_compaction::base}; plan_constraint_compaction_shader_index < ::game_state::shader_indices::tick::process_entities::pre_constraints::plan_compaction::end; ++plan_constraint_compaction_shader_index)
			{
				// TODO: REMOVE.
				if (plan_constraint_compaction_shader_index != static_cast<GLuint>(::game_state::shader_indices::tick::process_entities::pre_constraints::plan_compaction::Indices::plan_rigid_body_circle_contact_constraint_compaction))
				{
					continue;
				}

				glUseProgram(environment.state.shaders[plan_constraint_compaction_shader_index]);
				// TODO: We should exclude commit-count shaders.
				GLuint index_in_tick_entities_shader_array{ plan_constraint_compaction_shader_index - ::game_state::shader_indices::tick::process_entities::base };
				GLintptr command_offset
				{
					environment.state.layouts.commands.dispatch_commands_work_group_count_x_state.offset +
					index_in_tick_entities_shader_array * environment.state.layouts.commands.dispatch_commands_work_group_count_x_state.top_level_array_stride
				};
				glDispatchComputeIndirect(command_offset);
			}

			glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);*/

			/*if (environment.state.is_debugging)
			{
				debug::print_constraint_compaction_plan(environment);
			}*/

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
				//glDispatchComputeIndirect(command_offset);
			}

			glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

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
