#include "glad_glfw.h"
#include "game_logic/OpenGL_capabilities/include.h"

namespace game_logic::OpenGL_capabilities
{
	void query(game_state::OpenGL_capabilities::State& state)
	{
		glGetIntegerv(GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS, &state.max_compute_shader_storage_blocks);
		glGetIntegerv(GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS, &state.max_combined_shader_storage_blocks);
		glGetIntegerv(GL_MAX_COMPUTE_UNIFORM_BLOCKS, &state.max_compute_uniform_blocks);
		glGetIntegerv(GL_MAX_COMBINED_UNIFORM_BLOCKS, &state.max_combined_uniform_blocks);
		glGetIntegerv(GL_MAX_DRAW_BUFFERS, &state.max_draw_buffers);
		glGetIntegerv(GL_MAX_DUAL_SOURCE_DRAW_BUFFERS, &state.max_dual_source_draw_buffers);
		glGetIntegerv(GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS, &state.max_fragment_shader_storage_blocks);
		glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_BLOCKS, &state.max_fragment_uniform_blocks);
		glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &state.max_shader_storage_buffer_bindings);
		glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &state.max_uniform_buffer_bindings);
		glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &state.max_uniform_block_size);
		glGetIntegerv(GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS, &state.max_vertex_shader_storage_blocks);
		glGetIntegerv(GL_MAX_VERTEX_UNIFORM_BLOCKS, &state.max_vertex_uniform_blocks);
	}
}
