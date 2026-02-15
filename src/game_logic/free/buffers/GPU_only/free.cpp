#pragma once
#include "game_logic/free/buffers/GPU_only/include.h"
#include <iostream>

namespace game_logic::free::buffers::GPU_only
{
	void free(game_environment::Environment& environment)
	{
		glDeleteBuffers(environment.state.buffers.GPU_only.count, environment.state.buffers.GPU_only.buffers); 
		delete[] environment.state.buffers.GPU_only.buffers;
	}
}
