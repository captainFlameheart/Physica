#include "glad_glfw.h"
#include "game_logic/on_key_event/include.h"
#include "game_environment/environment.h"

namespace game_logic::on_mouse_button_event
{
	void on_mouse_button_event(
		game_environment::Environment& environment,
		int const button,
		int const action,
		int const mods
	)
	{
		glUseProgram(environment.state.shaders[static_cast<GLuint>(::game_state::shader_indices::on_mouse_button_event::Indices::on_mouse_button_event)]);

		GLuint aligned_size{
			::game_logic::binding_util::align_uniform_block_size(environment, environment.state.layouts.mouse_button_event.block_state.buffer_data_size)
		};
		GLuint offset{ environment.state.buffers.uniform.mouse_button_event_offset + environment.state.buffers.uniform.mouse_button_event_index * aligned_size };
		GLbyte* mapping{ environment.state.buffers.uniform.mapping + offset };

		GLsync fence{ environment.state.buffers.uniform.mouse_button_event_fences[environment.state.buffers.uniform.mouse_button_event_index] };
		GLenum fence_status{ glClientWaitSync(fence, GL_SYNC_FLUSH_COMMANDS_BIT, 0u) };
		while (fence_status != GL_ALREADY_SIGNALED && fence_status != GL_CONDITION_SATISFIED)
		{
			if (fence_status == GL_WAIT_FAILED)
			{
				std::cerr << "Failed to wait on mouse button event fence!" << std::endl;
			}
			if (fence_status == GL_TIMEOUT_EXPIRED)
			{
				std::cout << "Had to wait on mouse button event fence! Consider increasing capacity." << std::endl;
			}
			fence_status = glClientWaitSync(fence, 0u, 0u);
		}
		glDeleteSync(fence);

		GLuint button_action_mods[4u]{ button, action, mods, 0u };
		std::memcpy(mapping + environment.state.layouts.mouse_button_event.button_action_mods_state.offset, button_action_mods, sizeof(button_action_mods));

		glFlushMappedNamedBufferRange
		(
			environment.state.buffers.uniform.buffer, offset, environment.state.layouts.mouse_button_event.block_state.buffer_data_size
		);

		glBindBufferRange
		(
			GL_UNIFORM_BUFFER, static_cast<GLuint>(::game_state::bindings::uniform::bindings::private_input),
			environment.state.buffers.uniform.buffer, offset, environment.state.layouts.mouse_button_event.block_state.buffer_data_size
		);
		glDispatchCompute(1u, 1u, 1u);
		
		environment.state.buffers.uniform.mouse_button_event_fences[environment.state.buffers.uniform.mouse_button_event_index] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0u);
		environment.state.buffers.uniform.mouse_button_event_index =
			(environment.state.buffers.uniform.mouse_button_event_index + 1u) % ::game_state::buffers::uniform::mouse_button_event_capacity;

		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

		if (action == GLFW_PRESS)
		{
			switch (button)
			{
			case GLFW_MOUSE_BUTTON_LEFT:
				break;
			default:
				break;
			}
		}
	}
}
