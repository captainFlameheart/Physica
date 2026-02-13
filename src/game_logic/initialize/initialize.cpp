#include "game_logic/initialize/include.h"
#include "game_environment/environment.h"
#include <iostream>
#include "game_logic/OpenGL_capabilities/include.h"

namespace game_logic::initialize
{
	void initialize(game_environment::Environment& environment)
	{
		::game_logic::OpenGL_capabilities::query(environment.state.OpenGL_capabilities);
		::game_logic::OpenGL_capabilities::print(environment.state.OpenGL_capabilities);

		compile_shaders::compile_shaders(environment);
	}
}
