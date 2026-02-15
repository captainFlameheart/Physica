#include "game_logic/binding_util/include.h"
#include "game_environment/environment.h"
#include "game_state/bindings/include.h"

namespace game_logic::binding_util
{
	void bind_GPU_only_buffer(game_environment::Environment& environment, GLuint index)
	{
		GLuint buffer{ environment.state.buffers.GPU_only.buffers[index] };
		GLuint buffers[::game_state::bindings::shader_storage::count];
		for (GLuint i{ 0u }; i < ::game_state::bindings::shader_storage::count; ++i)
		{
			buffers[i] = buffer;
		}
		glBindBuffersBase
		(
			GL_SHADER_STORAGE_BUFFER, ::game_state::bindings::shader_storage::fixed_data, ::game_state::bindings::shader_storage::count, buffers
		);
		glBindBufferBase(GL_UNIFORM_BUFFER, ::game_state::bindings::uniform::fixed_data, buffer);
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, buffer);
	}
}