#pragma once
#include "game_logic/free/buffers/include.h"

namespace game_logic::free::buffers
{
	void free(game_environment::Environment& environment)
	{
		GPU_only::free(environment);
	}
}
