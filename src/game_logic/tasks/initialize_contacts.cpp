#pragma once
#include "glad_glfw.h"
#include "game_environment/environment.h"
#include "game_logic/tasks/include.h"

namespace game_logic::tasks
{
	void initialize_contacts(::game_environment::Environment& environment)
	{
		glUseProgram(environment.state.shaders[static_cast<GLuint>(::game_state::shader_indices::reusable::initialize_contacts::Indices::initialize_bounding_box_contact_detector_metadata)]);
		glDispatchCompute(1u, 1u, 1u);

		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_COMMAND_BARRIER_BIT);

		glUseProgram(environment.state.shaders[static_cast<GLuint>(::game_state::shader_indices::reusable::initialize_contacts::Indices::initialize_bounding_box_contact_detectors)]);
		glDispatchComputeIndirect
		(
			environment.state.layouts.commands.reusable_dispatch_commands_work_group_count_x_state.offset +
			0 * environment.state.layouts.commands.reusable_dispatch_commands_work_group_count_x_state.top_level_array_stride
		);
	}
}
