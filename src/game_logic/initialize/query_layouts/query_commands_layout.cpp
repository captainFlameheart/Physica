#include "game_logic/initialize/query_layouts/include.h"
#include "game_environment/Environment.h"
#include "game_logic/shader_util/query/include.h"
#include <iostream>

namespace game_logic::initialize::query_layouts
{
	void query_commands_layout(game_environment::Environment& environment, GLuint program)
	{
		::game_logic::shader_util::query::query_shader_storage_variable_offset_top_level_array_stride_top_level_array_size
		(
			program, "Commands.dispatch_commands[0].work_group_count_x",
			environment.state.layouts.commands.dispatch_commands_work_group_count_x_properties
		);
		::game_logic::shader_util::query::query_shader_storage_variable_offset
		(
			program, "Commands.dispatch_commands[0].work_group_count_y",
			environment.state.layouts.commands.dispatch_commands_work_group_count_y_properties
		);
		::game_logic::shader_util::query::query_shader_storage_variable_offset
		(
			program, "Commands.dispatch_commands[0].work_group_count_z",
			environment.state.layouts.commands.dispatch_commands_work_group_count_z_properties
		);

		::game_logic::shader_util::query::query_shader_storage_variable_offset_top_level_array_stride_top_level_array_size
		(
			program, "Commands.draw_arrays_commands[0].count",
			environment.state.layouts.commands.draw_arrays_commands_count_properties
		);
		::game_logic::shader_util::query::query_shader_storage_variable_offset
		(
			program, "Commands.draw_arrays_commands[0].instance_count",
			environment.state.layouts.commands.draw_arrays_commands_instance_count_properties
		);
		::game_logic::shader_util::query::query_shader_storage_variable_offset
		(
			program, "Commands.draw_arrays_commands[0].first",
			environment.state.layouts.commands.draw_arrays_commands_first_properties
		);
		::game_logic::shader_util::query::query_shader_storage_variable_offset
		(
			program, "Commands.draw_arrays_commands[0].base_instance",
			environment.state.layouts.commands.draw_arrays_commands_base_instance_properties
		);

		::game_logic::shader_util::query::query_shader_storage_buffer_data_size
		(
			program, "Commands",
			environment.state.layouts.commands.block_properties
		);
	}
}
