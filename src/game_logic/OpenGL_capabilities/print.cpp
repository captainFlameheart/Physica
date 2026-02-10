#include "glad_glfw.h"
#include "game_logic/OpenGL_capabilities/include.h"
#include <iostream>

namespace game_logic::OpenGL_capabilities
{
	void print(game_state::OpenGL_capabilities::State const& state)
	{
		std::cout << "game_logic::OpenGL_capabilities::State:\n"
			"	max_compute_shader_storage_blocks: " << state.max_compute_shader_storage_blocks << '\n' <<
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
