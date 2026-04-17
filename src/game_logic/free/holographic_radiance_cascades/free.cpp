#pragma once
#include "game_logic/free/holographic_radiance_cascades/include.h"
#include <iostream>

namespace game_logic::free::holographic_radiance_cascades
{
	void free(game_environment::Environment& environment)
	{
		std::cout << "Free holographic radiance cascades." << std::endl;

		deallocate(environment);
	}
}
