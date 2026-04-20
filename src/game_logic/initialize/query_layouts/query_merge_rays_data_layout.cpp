#include "game_logic/initialize/query_layouts/include.h"
#include "game_environment/Environment.h"
#include "game_logic/shader_util/query/include.h"
#include <iostream>

namespace game_logic::initialize::query_layouts
{
	void query_merge_rays_data_layout(game_environment::Environment& environment, GLuint program)
	{
		::game_logic::shader_util::query::query_uniform_variable_offset
		(
			program, "Merge_Rays_Data.rays_per_probe",
			environment.state.layouts.merge_rays_data.rays_per_probe_properties
		);
		::game_logic::shader_util::query::query_uniform_variable_offset
		(
			program, "Merge_Rays_Data.lower_cascade_rays_per_probe",
			environment.state.layouts.merge_rays_data.lower_cascade_rays_per_probe_properties
		);
		::game_logic::shader_util::query::query_uniform_variable_offset
		(
			program, "Merge_Rays_Data.lower_cascade_power_of_two",
			environment.state.layouts.merge_rays_data.lower_cascade_power_of_two_properties
		);
		::game_logic::shader_util::query::query_uniform_variable_offset
		(
			program, "Merge_Rays_Data.lower_cascade_max_probe_column_texel_x",
			environment.state.layouts.merge_rays_data.lower_cascade_max_probe_column_texel_x_properties
		);
		::game_logic::shader_util::query::query_uniform_variable_offset
		(
			program, "Merge_Rays_Data.lower_cascade_max_probe_row",
			environment.state.layouts.merge_rays_data.lower_cascade_max_probe_row_properties
		);
		::game_logic::shader_util::query::query_uniform_buffer_data_size
		(
			program, "Merge_Rays_Data",
			environment.state.layouts.merge_rays_data.block_properties
		);
	}
}
