#include "game_logic/binding_util/include.h"
#include "game_environment/environment.h"
#include "game_state/bindings/include.h"

namespace game_logic::binding_util
{
	void bind_GPU_only_buffer(game_environment::Environment& environment, GLuint index)
	{
		environment.state.buffers.GPU_only.current = index;
		GLuint buffer{ environment.state.buffers.GPU_only.buffers[index] };
		
		GLuint buffers[::game_state::bindings::shader_storage::commands];
		for (GLuint i{ 0u }; i < ::game_state::bindings::shader_storage::commands; ++i)
		{
			buffers[i] = buffer;
		}
		//buffers[::game_state::bindings::shader_storage::bindings::commands] = environment.state.buffers.GPU_only.command_buffer;
		glBindBuffersBase
		(
			GL_SHADER_STORAGE_BUFFER,
			::game_state::bindings::shader_storage::fixed_data,
			::game_state::bindings::shader_storage::commands,
			buffers
		);
		
		glBindBufferBase(GL_UNIFORM_BUFFER, ::game_state::bindings::uniform::fixed_data, buffer);
		
		//glBindBuffer(GL_DISPATCH_INDIRECT_BUFFER, buffer);
		//glBindBuffer(GL_DRAW_INDIRECT_BUFFER, buffer);
	}
}