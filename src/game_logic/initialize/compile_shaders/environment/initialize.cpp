#include "game_logic/initialize/compile_shaders/environment/include.h"
#include "game_environment/environment.h"
#include "game_state/device_requirements/OpenGL_versions.h"
#include <iostream>

namespace game_logic::initialize::compile_shaders::environment
{
	void initialize
	(
		game_environment::Environment& environment, ::game_state::initialize::compile_shaders::environment::Environment& compile_environment
	)
	{
		compile_environment.version_directive = "#version " + std::string{ ::game_state::device_requirements::OpenGL_versions::version_string };
	}
}
