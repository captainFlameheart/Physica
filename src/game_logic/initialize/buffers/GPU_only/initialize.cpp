#include "game_logic/initialize/buffers/GPU_only/include.h"
#include <iostream>

namespace game_logic::initialize::buffers::GPU_only
{
	void initialize(game_environment::Environment& environment)
	{
		environment.state.buffers.GPU_only.size = 128u * 1024u * 1024u;
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

		environment.state.buffers.GPU_only.command_buffer_size = environment.state.layouts.commands.block_state.buffer_data_size;
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
