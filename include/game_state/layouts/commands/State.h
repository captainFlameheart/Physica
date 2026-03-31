#pragma once
#include "glad_glfw.h"

namespace game_state::layouts::commands
{
	struct dispatch_commands_work_group_count_x
	{
		GLint offset;
		GLint top_level_array_stride;
		GLint top_level_array_size;
	};

	struct dispatch_commands_work_group_count_y
	{
		GLint offset;
	};
	
	struct dispatch_commands_work_group_count_z
	{
		GLint offset;
	};

	struct draw_arrays_commands_count
	{
		GLint offset;
		GLint top_level_array_stride;
		GLint top_level_array_size;
	};

	struct draw_arrays_commands_instance_count
	{
		GLint offset;
	};

	struct draw_arrays_commands_first
	{
		GLint offset;
	};

	struct draw_arrays_commands_base_instance
	{
		GLint offset;
	};

	struct remaining_dispatch_commands_work_group_count_x
	{
		GLint offset;
		GLint top_level_array_stride;
	};

	struct remaining_dispatch_commands_work_group_count_y
	{
		GLint offset;
	};

	struct remaining_dispatch_commands_work_group_count_z
	{
		GLint offset;
	};

	struct Block
	{
		GLint buffer_data_size;
	};

	struct State
	{
		union
		{
			dispatch_commands_work_group_count_x dispatch_commands_work_group_count_x_state;
			GLint dispatch_commands_work_group_count_x_properties[sizeof(dispatch_commands_work_group_count_x) / sizeof(GLint)];
		};
		union
		{
			dispatch_commands_work_group_count_y dispatch_commands_work_group_count_y_state;
			GLint dispatch_commands_work_group_count_y_properties[sizeof(dispatch_commands_work_group_count_y) / sizeof(GLint)];
		};
		union
		{
			dispatch_commands_work_group_count_z dispatch_commands_work_group_count_z_state;
			GLint dispatch_commands_work_group_count_z_properties[sizeof(dispatch_commands_work_group_count_z) / sizeof(GLint)];
		};

		union
		{
			draw_arrays_commands_count draw_arrays_commands_count_state;
			GLint draw_arrays_commands_count_properties[sizeof(draw_arrays_commands_count) / sizeof(GLint)];
		};
		union
		{
			draw_arrays_commands_instance_count draw_arrays_commands_instance_count_state;
			GLint draw_arrays_commands_instance_count_properties[sizeof(draw_arrays_commands_instance_count) / sizeof(GLint)];
		};
		union
		{
			draw_arrays_commands_first draw_arrays_commands_first_state;
			GLint draw_arrays_commands_first_properties[sizeof(draw_arrays_commands_first) / sizeof(GLint)];
		};
		union
		{
			draw_arrays_commands_base_instance draw_arrays_commands_base_instance_state;
			GLint draw_arrays_commands_base_instance_properties[sizeof(draw_arrays_commands_base_instance) / sizeof(GLint)];
		};

		union
		{
			remaining_dispatch_commands_work_group_count_x remaining_dispatch_commands_work_group_count_x_state;
			GLint remaining_dispatch_commands_work_group_count_x_properties[sizeof(remaining_dispatch_commands_work_group_count_x) / sizeof(GLint)];
		};
		union
		{
			remaining_dispatch_commands_work_group_count_y remaining_dispatch_commands_work_group_count_y_state;
			GLint remaining_dispatch_commands_work_group_count_y_properties[sizeof(remaining_dispatch_commands_work_group_count_y) / sizeof(GLint)];
		};
		union
		{
			remaining_dispatch_commands_work_group_count_z remaining_dispatch_commands_work_group_count_z_state;
			GLint remaining_dispatch_commands_work_group_count_z_properties[sizeof(remaining_dispatch_commands_work_group_count_z) / sizeof(GLint)];
		};

		union
		{
			Block block_state;
			GLint block_properties[sizeof(Block) / sizeof(GLint)];
		};
	};
}
