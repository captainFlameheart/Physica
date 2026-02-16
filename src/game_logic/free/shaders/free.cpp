#pragma once
#include "game_environment/environment.h"

namespace game_logic::free::shaders
{
	void free(game_environment::Environment& environment)
	{
		for (GLuint shader_index{ 0u }; shader_index < ::game_state::shader_indices::count; ++shader_index)
		{
			glDeleteProgram(environment.state.shaders[shader_index]);
		}
	}
}
