#include "game_logic/initialize/query_layouts/include.h"
#include "game_environment/Environment.h"
#include "game_logic/shader_util/query/include.h"
#include <iostream>

namespace game_logic::initialize::query_layouts
{
	void query_uvec2_data_layout(game_environment::Environment& environment, GLuint program)
	{
		::game_logic::shader_util::query::query_shader_storage_variable_offset_array_stride
		(
			program, "uvec2_Data.data",
			environment.state.layouts.uvec2_data.properties
		);
	}
}
