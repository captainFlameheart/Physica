#include "game_logic/initialize/query_layouts/include.h"
#include "game_environment/Environment.h"
#include "game_logic/shader_util/query/include.h"
#include <iostream>

namespace game_logic::initialize::query_layouts
{
	void query_merge_fluence_data_layout(game_environment::Environment& environment, GLuint program)
	{
		::game_logic::shader_util::query::query_uniform_variable_offset
		(
			program, "Merge_Fluence_Data.direction_mask",
			environment.state.layouts.merge_fluence_data.direction_mask_properties
		);
		::game_logic::shader_util::query::query_uniform_variable_offset
		(
			program, "Merge_Fluence_Data.cascade",
			environment.state.layouts.merge_fluence_data.cascade_properties
		);
		::game_logic::shader_util::query::query_uniform_variable_offset
		(
			program, "Merge_Fluence_Data.max_ray_probe_column",
			environment.state.layouts.merge_fluence_data.max_ray_probe_column_properties
		);
		::game_logic::shader_util::query::query_uniform_variable_offset
		(
			program, "Merge_Fluence_Data.max_fluence_probe_column_texel_x",
			environment.state.layouts.merge_fluence_data.max_fluence_probe_column_texel_x_properties
		);
		::game_logic::shader_util::query::query_uniform_variable_offset
		(
			program, "Merge_Fluence_Data.max_fluence_probe_y",
			environment.state.layouts.merge_fluence_data.max_fluence_probe_y_properties
		);
		::game_logic::shader_util::query::query_uniform_variable_offset
		(
			program, "Merge_Fluence_Data.rays_per_probe",
			environment.state.layouts.merge_fluence_data.rays_per_probe_properties
		);
		::game_logic::shader_util::query::query_uniform_variable_offset
		(
			program, "Merge_Fluence_Data.cascade_power_of_two",
			environment.state.layouts.merge_fluence_data.cascade_power_of_two_properties
		);
		::game_logic::shader_util::query::query_uniform_variable_offset
		(
			program, "Merge_Fluence_Data.upper_cascade_probe_column_texel_x_mask",
			environment.state.layouts.merge_fluence_data.upper_cascade_probe_column_texel_x_mask_properties
		);
		::game_logic::shader_util::query::query_uniform_variable_offset
		(
			program, "Merge_Fluence_Data.upper_cascade",
			environment.state.layouts.merge_fluence_data.upper_cascade_properties
		);
		::game_logic::shader_util::query::query_uniform_variable_offset
		(
			program, "Merge_Fluence_Data.upper_cascade_fluence_layer",
			environment.state.layouts.merge_fluence_data.upper_cascade_fluence_layer_properties
		);
		::game_logic::shader_util::query::query_uniform_variable_offset
		(
			program, "Merge_Fluence_Data.output_factor",
			environment.state.layouts.merge_fluence_data.output_factor_properties
		);
		::game_logic::shader_util::query::query_uniform_variable_offset
		(
			program, "Merge_Fluence_Data.output_shift",
			environment.state.layouts.merge_fluence_data.output_shift_properties
		);
		::game_logic::shader_util::query::query_uniform_buffer_data_size
		(
			program, "Merge_Fluence_Data",
			environment.state.layouts.merge_fluence_data.block_properties
		);
	}
}
