#pragma once
#include "game_state/initialize/compile_shaders/environment/include.h"
#include "game_environment/environment.h"

namespace game_logic::initialize::compile_shaders::environment
{
	std::string initialize_input_constants
	(
		game_environment::Environment& environment, ::game_state::initialize::compile_shaders::environment::Environment& compile_environment
	);
}
