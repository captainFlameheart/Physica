#pragma once
#include "game_logic/free/buffers/bounding_volume_hierarchy/include.h"
#include <iostream>

namespace game_logic::free::buffers::bounding_volume_hierarchy
{
	void free(game_environment::Environment& environment)
	{
		std::cout << "Free bounding volume hierarchy buffer" << std::endl;
		glDeleteBuffers(1u, &environment.state.buffers.bounding_volume_hierarchy.buffer);
		glDeleteSync(environment.state.buffers.bounding_volume_hierarchy.fence);
	}
}
