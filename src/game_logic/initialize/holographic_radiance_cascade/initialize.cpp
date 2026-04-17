#include "game_logic/initialize/holographic_radiance_cascades/include.h"
#include "glad_glfw.h"
#include "game_environment/Environment.h"
#include <array>

namespace game_logic::initialize::holographic_radiance_cascades
{
	void initialize(game_environment::Environment& environment)
	{
		std::cout << "Initialize Holographic Radiance Cascades." << std::endl;

		environment.state.holographic_radiance_cascades.enabled = true;
		environment.state.holographic_radiance_cascades.visible_source_layer = 0u;

		allocate(environment);
	}
}
