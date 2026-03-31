#include "game_logic/initialize/query_layouts/include.h"
#include "game_environment/Environment.h"
#include "game_logic/shader_util/query/include.h"
#include <iostream>

namespace game_logic::initialize::query_layouts
{
	void query_bounding_volume_hierarchy_layout(game_environment::Environment& environment, GLuint program)
	{
		::game_logic::shader_util::query::query_shader_storage_variable_offset
		(
			program, "Bounding_Volume_Hierarchy.height",
			environment.state.layouts.bounding_volume_hierarchy.height_properties
		);
		::game_logic::shader_util::query::query_shader_storage_buffer_data_size
		(
			program, "Bounding_Volume_Hierarchy",
			environment.state.layouts.bounding_volume_hierarchy.block_properties
		);
	}
}
