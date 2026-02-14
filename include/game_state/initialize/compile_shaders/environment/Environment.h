#pragma once
#include <string>
#include "game_state/shader_group/include.h"

namespace game_state::initialize::compile_shaders::environment
{
	struct Environment
	{
		::game_state::shader_group::Shader_Group shader_group;

		std::string version_directive;

		std::string readonly_constant_definitions;
		std::string writable_constant_definitions;

		std::string constant_definitions;

		std::string fixed_data_source;
		std::string uvec4_data_source;
		std::string uint_data_source;
		std::string float_data_source;
		
		std::string combined_data_source;

		std::string constants_and_data_source;

		std::string readonly_prefix_source;
		std::string writable_prefix_source;
	};
}
