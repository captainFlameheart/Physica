#include "game_logic/initialize/compile_shaders/environment/include.h"
#include "game_environment/environment.h"
#include "game_logic/shader_group/include.h"
#include "game_state/device_requirements/OpenGL_versions.h"
#include "util/shader/shader.h"
#include "game_state/bindings/include.h"
#include "game_state/local_sizes/include.h"
#include "game_state/vertex_factors/include.h"
#include "game_state/entity_type_indices/entity_type_indices.h"
#include "game_state/shader_to_entity_type/shader_to_entity_type.h"
#include "game_state/units/include.h"
#include "macros/macros.h"
#include <iostream>

namespace game_logic::initialize::compile_shaders::environment
{
	std::string initialize_input_constants
	(
		game_environment::Environment& environment, ::game_state::initialize::compile_shaders::environment::Environment& compile_environment
	)
	{
		return (
			"const uint key_space = " STRINGIFY(GLFW_KEY_SPACE) ";\n"
		);
	}
}
