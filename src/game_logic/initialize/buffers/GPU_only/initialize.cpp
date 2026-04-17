#include "game_logic/initialize/buffers/GPU_only/include.h"
#include <iostream>

namespace game_logic::initialize::buffers::GPU_only
{
	void initialize(game_environment::Environment& environment)
	{
		constexpr GLuint size_factor{ 10u };

		environment.state.buffers.GPU_only.size = size_factor * 128u * 1024u * 1024u;
		environment.state.buffers.GPU_only.count = 1u;
		environment.state.buffers.GPU_only.buffers = new GLuint[environment.state.buffers.GPU_only.count];

		glCreateBuffers(environment.state.buffers.GPU_only.count, environment.state.buffers.GPU_only.buffers);

		for (GLuint buffer_index{ 0u }; buffer_index < environment.state.buffers.GPU_only.count; ++buffer_index)
		{
			glNamedBufferStorage
			(
				environment.state.buffers.GPU_only.buffers[buffer_index],
				environment.state.buffers.GPU_only.size,
				nullptr,
				0u
			);
		}

		// MUST TODO: Update command_buffer_remaining_dispatch_commands_capacity during capacity update!
		environment.state.buffers.GPU_only.command_buffer_remaining_dispatch_commands_capacity = 1000000u;
		environment.state.buffers.GPU_only.command_buffer_size =
			environment.state.layouts.commands.remaining_dispatch_commands_work_group_count_x_state.offset +
			(
				environment.state.buffers.GPU_only.command_buffer_remaining_dispatch_commands_capacity *
				environment.state.layouts.commands.remaining_dispatch_commands_work_group_count_x_state.top_level_array_stride
			);
		glCreateBuffers(1u, &environment.state.buffers.GPU_only.command_buffer);
		glNamedBufferStorage
		(
			environment.state.buffers.GPU_only.command_buffer,
			environment.state.buffers.GPU_only.command_buffer_size,
			nullptr,
			0u
		);

		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, ::game_state::bindings::shader_storage::commands, environment.state.buffers.GPU_only.command_buffer);
		glBindBuffer(GL_DISPATCH_INDIRECT_BUFFER, environment.state.buffers.GPU_only.command_buffer);
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, environment.state.buffers.GPU_only.command_buffer);
	}
}
