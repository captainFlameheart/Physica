#include "game_logic/initialize/compile_shaders/compile_shaders.h"
#include "glad_glfw.h"
#include "game_environment/environment.h"
#include "util/shader/shader.h"
#include "game_logic/initialize/compile_shaders/environment/include.h"
#include <iostream>

namespace game_logic::initialize::compile_shaders
{
	void compile_shaders(game_environment::Environment& environment)
	{
		::game_state::initialize::compile_shaders::environment::Environment compile_environment;
		environment::initialize(environment, compile_environment);

		std::cout << "Here" << std::endl;

		environment::free(environment, compile_environment);
	}
}
