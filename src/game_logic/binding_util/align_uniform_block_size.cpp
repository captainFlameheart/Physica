#include "game_logic/binding_util/include.h"
#include "game_environment/environment.h"

namespace game_logic::binding_util
{
	GLuint align_uniform_block_size(game_environment::Environment& environment, GLuint uniform_block_size)
	{
		return align_block_size(environment, uniform_block_size, environment.state.OpenGL_capabilities.uniform_buffer_offset_alignment);
	}
}
