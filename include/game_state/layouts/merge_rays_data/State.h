#pragma once

namespace game_state::layouts::merge_rays_data
{
	struct rays_per_probe
	{
		GLint offset;
	};

	struct lower_cascade_rays_per_probe
	{
		GLint offset;
	};

	struct lower_cascade_power_of_two
	{
		GLint offset;
	};

	struct lower_cascade_max_probe_column_texel_x
	{
		GLint offset;
	};

	struct lower_cascade_max_probe_row
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
			rays_per_probe rays_per_probe_state;
			GLint rays_per_probe_properties[sizeof(rays_per_probe) / sizeof(GLint)];
		};
		union
		{
			lower_cascade_rays_per_probe lower_cascade_rays_per_probe_state;
			GLint lower_cascade_rays_per_probe_properties[sizeof(lower_cascade_rays_per_probe) / sizeof(GLint)];
		};
		union
		{
			lower_cascade_power_of_two lower_cascade_power_of_two_state;
			GLint lower_cascade_power_of_two_properties[sizeof(lower_cascade_power_of_two) / sizeof(GLint)];
		};
		union
		{
			lower_cascade_max_probe_column_texel_x lower_cascade_max_probe_column_texel_x_state;
			GLint lower_cascade_max_probe_column_texel_x_properties[sizeof(lower_cascade_max_probe_column_texel_x) / sizeof(GLint)];
		};
		union
		{
			lower_cascade_max_probe_row lower_cascade_max_probe_row_state;
			GLint lower_cascade_max_probe_row_properties[sizeof(lower_cascade_max_probe_row) / sizeof(GLint)];
		};
		union
		{
			Block block_state;
			GLint block_properties[sizeof(Block) / sizeof(GLint)];
		};
	};
}
