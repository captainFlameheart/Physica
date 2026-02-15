#pragma once
#include "game_environment/environment.h"

namespace game_logic::free::shaders
{
	void free(game_environment::Environment& environment)
	{
		glDeleteProgram(environment.state.shaders.state.tick.state.bodies.state.point_masses.state.process);
		glDeleteProgram(environment.state.shaders.state.draw.state.bodies.state.point_masses.state.draw);
	}
}
