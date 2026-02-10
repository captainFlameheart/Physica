#include "glad_glfw.h"
#include "game_logic/OpenGL_capabilities/include.h"

namespace game_logic::OpenGL_capabilities
{
	void query(game_state::OpenGL_capabilities::State& state)
	{
		glGetIntegerv(GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS, &state.max_compute_shader_storage_blocks);
	}
}
