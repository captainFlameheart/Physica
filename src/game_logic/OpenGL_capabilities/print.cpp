#include "glad_glfw.h"
#include "game_logic/OpenGL_capabilities/include.h"
#include <iostream>

namespace game_logic::OpenGL_capabilities
{
	void print(game_state::OpenGL_capabilities::State const& state)
	{
		std::cout << "game_logic::OpenGL_capabilities::State:\n"
			"	max_compute_shader_storage_blocks: " << state.max_compute_shader_storage_blocks << '\n' <<
			"	max_combined_shader_storage_blocks: " << state.max_combined_shader_storage_blocks << '\n' <<
			"	max_compute_uniform_blocks: " << state.max_compute_uniform_blocks << '\n' <<
			"	max_combined_uniform_blocks: " << state.max_combined_uniform_blocks << '\n' <<
			"	max_draw_buffers: " << state.max_draw_buffers << '\n' <<
			"	max_dual_source_draw_buffers: " << state.max_dual_source_draw_buffers << '\n' <<
			"	max_fragment_shader_storage_blocks: " << state.max_fragment_shader_storage_blocks << '\n' <<
			"	max_fragment_uniform_blocks: " << state.max_fragment_uniform_blocks << '\n' <<
			"	max_shader_storage_buffer_bindings: " << state.max_shader_storage_buffer_bindings << '\n' <<
			"	max_uniform_buffer_bindings: " << state.max_uniform_buffer_bindings << '\n' <<
			"	max_uniform_block_size: " << state.max_uniform_block_size << '\n' <<
			"	max_vertex_shader_storage_blocks: " << state.max_vertex_shader_storage_blocks << '\n' <<
			"	max_vertex_uniform_blocks: " << state.max_vertex_uniform_blocks << '\n' <<
			std::endl;
	}
}

/*	TODO:
* a:
*	b: 2
*	c:
*		d: 3
*		e: 4
*		f:
		[
			2,
			3,
*		]
*/
