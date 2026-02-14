#include "game_logic/initialize/query_layouts/include.h"
#include "game_environment/Environment.h"
#include "game_logic/shader_util/query/include.h"
#include <iostream>

namespace game_logic::initialize::query_layouts
{
	void query_uvec4_data_layout(game_environment::Environment& environment)
	{
		::game_logic::shader_util::query::query_shader_storage_variable_offset_array_stride
		(
			environment.state.shaders.state.tick.state.bodies.state.point_masses.state.process, "uvec4_Data.data",
			environment.state.layouts.uvec4_data.properties
		);
	}
}
