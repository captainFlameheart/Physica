#include "game_logic/initialize/compile_shaders/include.h"
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

		compile_initialize(environment, compile_environment);
		compile_process_point_masses(environment, compile_environment);
		compile_draw_point_masses(environment, compile_environment);

		environment::free(environment, compile_environment);
	}
}
