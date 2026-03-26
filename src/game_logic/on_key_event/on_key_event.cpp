#include "glad_glfw.h"
#include "game_logic/on_key_event/include.h"
#include "game_environment/environment.h"

namespace game_logic::on_key_event
{
	void on_key_event(
		game_environment::Environment& environment,
		int const key,
		int const scancode,
		int const action,
		int const mods
	)
	{
		glUseProgram(environment.state.shaders[static_cast<GLuint>(::game_state::shader_indices::on_key_event::Indices::on_key_event)]);

		GLuint aligned_size{
			::game_logic::binding_util::align_uniform_block_size(environment, environment.state.layouts.key_event.block_state.buffer_data_size)
		};
		GLuint offset{ environment.state.buffers.uniform.key_event_offset + environment.state.buffers.uniform.key_event_index * aligned_size };
		GLbyte* mapping{ environment.state.buffers.uniform.mapping + offset };

		GLsync fence{ environment.state.buffers.uniform.key_event_fences[environment.state.buffers.uniform.key_event_index] };
		GLenum fence_status{ glClientWaitSync(fence, GL_SYNC_FLUSH_COMMANDS_BIT, 0u) };
		while (fence_status != GL_ALREADY_SIGNALED && fence_status != GL_CONDITION_SATISFIED)
		{
			if (fence_status == GL_WAIT_FAILED)
			{
				std::cerr << "Failed to wait on key event fence!" << std::endl;
			}
			if (fence_status == GL_TIMEOUT_EXPIRED)
			{
				std::cout << "Had to wait on key event fence! Consider increasing capacity." << std::endl;
			}
			fence_status = glClientWaitSync(fence, 0u, 0u);
		}
		glDeleteSync(fence);

		GLuint key_scancode_action_mods[4u]{ key, scancode, action, mods };
		std::memcpy(mapping + environment.state.layouts.key_event.key_scancode_action_mods_state.offset, key_scancode_action_mods, sizeof(key_scancode_action_mods));

		glFlushMappedNamedBufferRange
		(
			environment.state.buffers.uniform.buffer, offset, environment.state.layouts.key_event.block_state.buffer_data_size
		);

		glBindBufferRange
		(
			GL_UNIFORM_BUFFER, static_cast<GLuint>(::game_state::bindings::uniform::bindings::private_input),
			environment.state.buffers.uniform.buffer, offset, environment.state.layouts.key_event.block_state.buffer_data_size
		);
		glDispatchCompute(1u, 1u, 1u);
		
		environment.state.buffers.uniform.key_event_fences[environment.state.buffers.uniform.key_event_index] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0u);
		environment.state.buffers.uniform.key_event_index =
			(environment.state.buffers.uniform.key_event_index + 1u) % ::game_state::buffers::uniform::key_event_capacity;

		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

		if (action == GLFW_PRESS)
		{
			switch (key)
			{
			case GLFW_KEY_P:
				environment.state.is_debugging = !environment.state.is_debugging;
				if (environment.state.is_debugging)
				{
					std::cout << "Debug started." << std::endl;
				}
				else
				{
					std::cout << "Debug ended." << std::endl;
				}
				break;
			case GLFW_KEY_O:
				::game_logic::debug::print_fixed_data(environment);
				break;
			default:
				break;
			}
		}
	}
}
