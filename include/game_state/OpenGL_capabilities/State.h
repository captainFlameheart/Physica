#pragma once

namespace game_state::OpenGL_capabilities
{
	struct State
	{
		GLint max_compute_shader_storage_blocks;
		GLint max_combined_shader_storage_blocks;
		GLint max_compute_uniform_blocks;
		GLint max_combined_uniform_blocks;
		GLint max_draw_buffers;
		GLint max_dual_source_draw_buffers;
		GLint max_fragment_shader_storage_blocks;
		GLint max_fragment_uniform_blocks;
		GLint max_shader_storage_buffer_bindings;
		GLint max_uniform_buffer_bindings;
		GLint max_uniform_block_size;
		GLint max_vertex_shader_storage_blocks;
		GLint max_vertex_uniform_blocks;
	};
}
