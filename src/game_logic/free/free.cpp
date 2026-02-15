#pragma once
#include "game_logic/free/include.h"

namespace game_logic::free
{
	void free(game_environment::Environment& environment)
	{
		glDeleteVertexArrays(1u, &environment.state.vertex_array);
		shaders::free(environment);
		buffers::free(environment);
	}
}
