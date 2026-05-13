#include "game_logic/initialize/query_layouts/include.h"
#include "game_environment/Environment.h"
#include "game_logic/shader_util/query/include.h"
#include <iostream>

namespace game_logic::initialize::query_layouts
{
	void query_timestamp_metadata_layout(game_environment::Environment& environment, GLuint program)
	{
		::game_logic::shader_util::query::query_uniform_variable_offset
		(
			program, "Timing_Configuration.timestamp_capacity",
			environment.state.layouts.timestamp_metadata.timestamp_capacity_properties
		);
		::game_logic::shader_util::query::query_uniform_variable_offset
		(
			program, "Timing_Configuration.generation_capacity",
			environment.state.layouts.timestamp_metadata.generation_capacity_properties
		);
		::game_logic::shader_util::query::query_uniform_variable_offset
		(
			program, "Timing_Configuration.query_capacity",
			environment.state.layouts.timestamp_metadata.query_capacity_properties
		);
		::game_logic::shader_util::query::query_uniform_variable_offset
		(
			program, "Timing_Configuration.generation_query_capacity",
			environment.state.layouts.timestamp_metadata.generation_query_capacity_properties
		);

		::game_logic::shader_util::query::query_uniform_variable_offset
		(
			program, "Timing_Metadata.generation",
			environment.state.layouts.timestamp_metadata.generation_properties
		);
		::game_logic::shader_util::query::query_uniform_variable_offset
		(
			program, "Timing_Metadata.next_timestamp",
			environment.state.layouts.timestamp_metadata.next_timestamp_properties
		);

		::game_logic::shader_util::query::query_uniform_variable_offset_top_level_array_stride
		(
			program, "Timestamp_Metadata.metadata[0].color",
			environment.state.layouts.timestamp_metadata.metadata_color_properties
		);
		::game_logic::shader_util::query::query_uniform_variable_offset
		(
			program, "Timestamp_Metadata.metadata[0].generation",
			environment.state.layouts.timestamp_metadata.metadata_generation_properties
		);
		::game_logic::shader_util::query::query_uniform_variable_offset
		(
			program, "Timestamp_Metadata.metadata[0].name_base",
			environment.state.layouts.timestamp_metadata.metadata_name_base_properties
		);
	}
}
