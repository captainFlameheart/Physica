#pragma once

namespace game_state::layouts::merge_fluence_data
{
	struct direction_mask
	{
		GLint offset;
	};

	struct cascade
	{
		GLint offset;
	};

	struct max_ray_probe_column
	{
		GLint offset;
	};

	struct max_fluence_probe_column_texel_x
	{
		GLint offset;
	};

	struct max_fluence_probe_y
	{
		GLint offset;
	};

	struct rays_per_probe
	{
		GLint offset;
	};

	struct cascade_power_of_two
	{
		GLint offset;
	};

	struct upper_cascade_probe_column_texel_x_mask
	{
		GLint offset;
	};

	struct upper_cascade
	{
		GLint offset;
	};

	struct upper_cascade_fluence_layer
	{
		GLint offset;
	};

	struct output_factor
	{
		GLint offset;
	};

	struct output_shift
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
			direction_mask direction_mask_state;
			GLint direction_mask_properties[sizeof(direction_mask) / sizeof(GLint)];
		};
		union
		{
			cascade cascade_state;
			GLint cascade_properties[sizeof(cascade) / sizeof(GLint)];
		};
		union
		{
			max_ray_probe_column max_ray_probe_column_state;
			GLint max_ray_probe_column_properties[sizeof(max_ray_probe_column) / sizeof(GLint)];
		};
		union
		{
			max_fluence_probe_column_texel_x max_fluence_probe_column_texel_x_state;
			GLint max_fluence_probe_column_texel_x_properties[sizeof(max_fluence_probe_column_texel_x) / sizeof(GLint)];
		};
		union
		{
			max_fluence_probe_y max_fluence_probe_y_state;
			GLint max_fluence_probe_y_properties[sizeof(max_fluence_probe_y) / sizeof(GLint)];
		};
		union
		{
			rays_per_probe rays_per_probe_state;
			GLint rays_per_probe_properties[sizeof(rays_per_probe) / sizeof(GLint)];
		};
		union
		{
			cascade_power_of_two cascade_power_of_two_state;
			GLint cascade_power_of_two_properties[sizeof(cascade_power_of_two) / sizeof(GLint)];
		};
		union
		{
			upper_cascade_probe_column_texel_x_mask upper_cascade_probe_column_texel_x_mask_state;
			GLint upper_cascade_probe_column_texel_x_mask_properties[sizeof(upper_cascade_probe_column_texel_x_mask) / sizeof(GLint)];
		};
		union
		{
			upper_cascade upper_cascade_state;
			GLint upper_cascade_properties[sizeof(upper_cascade) / sizeof(GLint)];
		};
		union
		{
			upper_cascade_fluence_layer upper_cascade_fluence_layer_state;
			GLint upper_cascade_fluence_layer_properties[sizeof(upper_cascade_fluence_layer) / sizeof(GLint)];
		};
		union
		{
			output_factor output_factor_state;
			GLint output_factor_properties[sizeof(output_factor) / sizeof(GLint)];
		};
		union
		{
			output_shift output_shift_state;
			GLint output_shift_properties[sizeof(output_shift) / sizeof(GLint)];
		};
		union
		{
			Block block_state;
			GLint block_properties[sizeof(Block) / sizeof(GLint)];
		};
	};
}
