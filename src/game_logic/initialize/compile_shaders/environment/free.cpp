#include "game_logic/initialize/compile_shaders/environment/include.h"
#include "game_environment/environment.h"
#include "game_logic/shader_group/include.h"
#include "game_state/device_requirements/OpenGL_versions.h"

namespace game_logic::initialize::compile_shaders::environment
{
	void free
	(
		game_environment::Environment& environment, ::game_state::initialize::compile_shaders::environment::Environment& compile_environment
	)
	{
		::game_logic::shader_group::free(compile_environment.shader_group);
	}
}
