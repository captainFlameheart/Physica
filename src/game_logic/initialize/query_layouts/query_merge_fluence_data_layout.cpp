#include "game_logic/initialize/query_layouts/include.h"
#include "game_environment/Environment.h"
#include "game_logic/shader_util/query/include.h"
#include <iostream>

namespace game_logic::initialize::query_layouts
{
	void query_merge_fluence_data_layout(game_environment::Environment& environment, GLuint program)
	{
		::game_logic::shader_util::query::query_uniform_buffer_data_size
		(
			program, "Merge_Fluence_Data",
			environment.state.layouts.merge_fluence_data.block_properties
		);
	}
}
