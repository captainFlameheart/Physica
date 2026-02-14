#pragma once
#include "glad_glfw.h"
#include "game_environment/environment.h"
#include "game_state/initialize/compile_shaders/environment/include.h"

namespace game_logic::initialize::compile_shaders
{
	void compile_draw_point_masses
	(
		game_environment::Environment& environment, ::game_state::initialize::compile_shaders::environment::Environment compile_environment
	);
}
