#include "game_logic/initialize/buffers/uniform/initialize.h"
#include "game_environment/environment.h"
#include "game_logic/binding_util/include.h"
#include <iostream>

namespace game_logic::initialize::buffers::uniform
{
	void initialize(game_environment::Environment& environment)
	{
		environment.state.buffers.uniform.key_event_offset = 0u;
		GLuint aligned_key_event_size{ binding_util::align_uniform_block_size(environment, environment.state.layouts.key_event.block_state.buffer_data_size) };
		GLuint full_aligned_key_event_size{ aligned_key_event_size * ::game_state::buffers::uniform::key_event_capacity };

		GLuint buffer_size{ full_aligned_key_event_size };

		environment.state.buffers.uniform.on_events_polled_offset = buffer_size;
		GLuint aligned_on_events_polled_size{ binding_util::align_uniform_block_size(environment, environment.state.layouts.on_events_polled_data.block_state.buffer_data_size) };
		GLuint full_aligned_on_events_polled_size{ aligned_on_events_polled_size * ::game_state::buffers::uniform::on_events_polled_capacity };

		buffer_size += full_aligned_on_events_polled_size;

		glCreateBuffers(1u, &environment.state.buffers.uniform.buffer);
		glNamedBufferStorage(environment.state.buffers.uniform.buffer, buffer_size, nullptr, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT);
		environment.state.buffers.uniform.mapping = static_cast<GLbyte*>
		(
			glMapNamedBufferRange(environment.state.buffers.uniform.buffer, static_cast<GLintptr>(0), buffer_size, GL_MAP_WRITE_BIT | GL_MAP_FLUSH_EXPLICIT_BIT | GL_MAP_PERSISTENT_BIT)
		);

		environment.state.buffers.uniform.key_event_index = 0u;

		for (GLuint i{ 0u }; i < ::game_state::buffers::uniform::key_event_capacity; ++i)
		{
			environment.state.buffers.uniform.key_event_fences[i] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0u);
		}

		environment.state.buffers.uniform.on_events_polled_index = 0u;

		for (GLuint i{ 0u }; i < ::game_state::buffers::uniform::on_events_polled_capacity; ++i)
		{
			environment.state.buffers.uniform.on_events_polled_fences[i] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0u);
		}
	}
}
