#pragma once
#include "game_logic/free/include.h"

namespace game_logic::free
{
	void free(game_environment::Environment& environment)
	{
		shaders::free(environment);
		buffers::free(environment);
	}
}
