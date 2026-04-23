#include "glad_glfw.h"
#include "game_logic/on_events_polled/include.h"
#include "game_environment/environment.h"

namespace game_logic::on_events_polled
{
	void on_events_polled(game_environment::Environment& environment)
	{
		double cursor_x_double;
		double cursor_y_double;
		glfwGetCursorPos(environment.window, &cursor_x_double, &cursor_y_double);
		GLfloat cursor_x{ static_cast<GLfloat>(cursor_x_double) };
		GLfloat cursor_y{ static_cast<GLfloat>(cursor_y_double) };
		
		glUseProgram(environment.state.shaders[static_cast<GLuint>(::game_state::shader_indices::on_events_polled::Indices::on_events_polled)]);

		GLuint aligned_size{
			::game_logic::binding_util::align_uniform_block_size(environment, environment.state.layouts.on_events_polled_data.block_state.buffer_data_size)
		};
		GLuint offset{ environment.state.buffers.uniform.on_events_polled_offset + environment.state.buffers.uniform.on_events_polled_index * aligned_size };
		GLbyte* mapping{ environment.state.buffers.uniform.mapping + offset };

		GLsync fence{ environment.state.buffers.uniform.on_events_polled_fences[environment.state.buffers.uniform.on_events_polled_index] };
		GLenum fence_status{ glClientWaitSync(fence, GL_SYNC_FLUSH_COMMANDS_BIT, 0u) };
		while (fence_status != GL_ALREADY_SIGNALED && fence_status != GL_CONDITION_SATISFIED)
		{
			if (fence_status == GL_WAIT_FAILED)
			{
				std::cerr << "Failed to wait on on-events-polled fence!" << std::endl;
			}
			if (fence_status == GL_TIMEOUT_EXPIRED)
			{
				std::cout << "Had to wait on on-events-polled fence! Consider increasing capacity." << std::endl;
			}
			fence_status = glClientWaitSync(fence, 0u, 0u);
		}
		glDeleteSync(fence);

		GLfloat cursor_position_framebuffer_size[4u]{ cursor_x, cursor_y, environment.state.glfw.framebuffer_width, environment.state.glfw.framebuffer_height };
		std::memcpy(mapping + environment.state.layouts.on_events_polled_data.cursor_position_framebuffer_size_state.offset, cursor_position_framebuffer_size, sizeof(cursor_position_framebuffer_size));

		glFlushMappedNamedBufferRange
		(
			environment.state.buffers.uniform.buffer, offset, environment.state.layouts.on_events_polled_data.block_state.buffer_data_size
		);

		glBindBufferRange
		(
			GL_UNIFORM_BUFFER, static_cast<GLuint>(::game_state::bindings::uniform::bindings::private_input),
			environment.state.buffers.uniform.buffer, offset, environment.state.layouts.on_events_polled_data.block_state.buffer_data_size
		);
		glDispatchCompute(1u, 1u, 1u);
		
		environment.state.buffers.uniform.on_events_polled_fences[environment.state.buffers.uniform.on_events_polled_index] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0u);
		environment.state.buffers.uniform.on_events_polled_index =
			(environment.state.buffers.uniform.on_events_polled_index + 1u) % ::game_state::buffers::uniform::on_events_polled_capacity;

		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	}
}
