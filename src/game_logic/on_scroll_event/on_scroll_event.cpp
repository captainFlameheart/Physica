#include "glad_glfw.h"
#include "game_logic/on_scroll_event/include.h"
#include "game_environment/environment.h"

namespace game_logic::on_scroll_event
{
	void on_scroll_event(
		game_environment::Environment& environment,
		double x_offset,
		double y_offset
	)
	{
		glUseProgram(environment.state.shaders[static_cast<GLuint>(::game_state::shader_indices::on_scroll_event::Indices::on_scroll_event)]);

		GLuint aligned_size{
			::game_logic::binding_util::align_uniform_block_size(environment, environment.state.layouts.scroll_event.block_state.buffer_data_size)
		};
		GLuint offset{ environment.state.buffers.uniform.scroll_event_offset + environment.state.buffers.uniform.scroll_event_index * aligned_size };
		GLbyte* mapping{ environment.state.buffers.uniform.mapping + offset };

		GLsync fence{ environment.state.buffers.uniform.scroll_event_fences[environment.state.buffers.uniform.scroll_event_index] };
		GLenum fence_status{ glClientWaitSync(fence, GL_SYNC_FLUSH_COMMANDS_BIT, 0u) };
		while (fence_status != GL_ALREADY_SIGNALED && fence_status != GL_CONDITION_SATISFIED)
		{
			if (fence_status == GL_WAIT_FAILED)
			{
				std::cerr << "Failed to wait on scroll event fence!" << std::endl;
			}
			if (fence_status == GL_TIMEOUT_EXPIRED)
			{
				std::cout << "Had to wait on scroll event fence! Consider increasing capacity." << std::endl;
			}
			fence_status = glClientWaitSync(fence, 0u, 0u);
		}
		glDeleteSync(fence);

		GLuint offsets[2u]{ x_offset, y_offset };
		std::memcpy(mapping + environment.state.layouts.scroll_event.offsets_state.offset, offsets, sizeof(offsets));

		glFlushMappedNamedBufferRange
		(
			environment.state.buffers.uniform.buffer, offset, environment.state.layouts.scroll_event.block_state.buffer_data_size
		);

		glBindBufferRange
		(
			GL_UNIFORM_BUFFER, static_cast<GLuint>(::game_state::bindings::uniform::bindings::private_input),
			environment.state.buffers.uniform.buffer, offset, environment.state.layouts.scroll_event.block_state.buffer_data_size
		);
		glDispatchCompute(1u, 1u, 1u);
		
		environment.state.buffers.uniform.scroll_event_fences[environment.state.buffers.uniform.scroll_event_index] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0u);
		environment.state.buffers.uniform.scroll_event_index =
			(environment.state.buffers.uniform.scroll_event_index + 1u) % ::game_state::buffers::uniform::scroll_event_capacity;

		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	}
}
