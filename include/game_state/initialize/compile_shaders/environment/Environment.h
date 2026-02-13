#pragma once
#include <string>
#include "game_state/shader_group/include.h"

namespace game_state::initialize::compile_shaders::environment
{
	struct Environment
	{
		::game_state::shader_group::Shader_Group shader_group;

		std::string version_directive;
		std::string fixed_data_source;
	};
}
