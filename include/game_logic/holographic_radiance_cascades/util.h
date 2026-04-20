#pragma once
#include "glad_glfw.h"
#include <algorithm>
#include "global/include.h"
#include "game_environment/environment.h"

namespace game_logic::holographic_radiance_cascades
{
	constexpr GLuint compute_orthogonal_bidirection
	(
		GLuint bidirection
	)
	{
		return bidirection ^ 1u;
	}

	constexpr GLuint compute_cascade_power_of_two
	(
		GLuint cascade
	)
	{
		return 1u << cascade;
	}

	constexpr GLuint compute_rays_per_probe
	(
		GLuint cascade_power_of_two
	)
	{
		return cascade_power_of_two + 1u;
	}

	constexpr GLuint compute_lower_cascade
	(
		GLuint cascade
	)
	{
		return cascade - 1u;
	}

	constexpr GLuint compute_upper_cascade
	(
		GLuint cascade
	)
	{
		return cascade + 1u;
	}

	constexpr GLuint compute_max_probe_column
	(
		GLuint probe_grid_length, GLuint cascade_power_of_two
	)
	{
		return ((probe_grid_length - 2u + cascade_power_of_two) / cascade_power_of_two) - 2u;
	}

	constexpr GLuint compute_max_probe_column_texel_x
	(
		GLuint max_probe_column, GLuint rays_per_probe
	)
	{
		return max_probe_column * rays_per_probe;
	}

	constexpr GLuint compute_max_probe_row
	(
		GLuint orhogonal_probe_grid_length
	)
	{
		return orhogonal_probe_grid_length - 1u;
	}

	constexpr GLuint compute_direction_mask
	(
		GLuint cascade_power_of_two
	)
	{
		return cascade_power_of_two - 1u;
	}

	constexpr GLuint compute_max_ray_probe_column
	(
		GLuint probe_grid_length, GLuint cascade_power_of_two
	)
	{
		return compute_max_probe_column(probe_grid_length, cascade_power_of_two);
	}

	constexpr GLuint compute_max_fluence_probe_column_texel_x
	(
		GLuint probe_grid_length, GLuint upper_cascade_power_of_two, GLuint upper_cascade
	)
	{
		// TODO: Is this correct?
		return ((probe_grid_length - 2u) >> upper_cascade) * upper_cascade_power_of_two;
	}

	constexpr GLuint compute_max_fluence_probe_y
	(
		GLuint orthogonal_probe_grid_length
	)
	{
		return orthogonal_probe_grid_length - 1u;
	}

	constexpr GLuint compute_upper_cascade_probe_column_texel_x_mask
	(
		GLuint upper_cascade
	)
	{
		return minus_1_uint << upper_cascade;
	}

	constexpr GLuint compute_fluence_layer
	(
		GLuint reversed_cascade
	)
	{
		return reversed_cascade & 1u;
	}

	constexpr GLuint compute_fluence_layer
	(
		GLuint directional_cascade_count, GLuint cascade
	)
	{
		return compute_fluence_layer(directional_cascade_count - cascade);
	}

	constexpr GLuint compute_upper_cascade_fluence_layer
	(
		GLuint directional_cascade_count, GLuint upper_cascade
	)
	{
		return compute_fluence_layer(directional_cascade_count, upper_cascade);
	}

	constexpr GLint compute_output_factor_for_last_cascade
	(
		GLuint direction
	)
	{
		return 1 - 2 * static_cast<GLint>(direction);
	}

	constexpr GLint compute_output_shift_for_last_cascade
	(
		GLuint direction, GLuint probe_grid_length
	)
	{
		// TODO: Is this correct?
		return direction * (probe_grid_length - 1u);
	}

	constexpr GLint compute_output_factor
	(
		GLuint cascade, GLuint direction
	)
	{
		if (cascade != 0u)
		{
			return 1u;
		}
		return compute_output_factor_for_last_cascade(direction);
	}

	constexpr GLint compute_output_shift
	(
		GLuint cascade, GLuint direction, GLuint probe_grid_length
	)
	{
		if (cascade != 0u)
		{
			return 1u;
		}
		return compute_output_shift_for_last_cascade(direction, probe_grid_length);
	}

	constexpr GLfloat compute_source_sample_point_to_probe_grid_point_factor
	(
		GLuint source_length
	)
	{
		return 1.0 - 1.0 / static_cast<GLfloat>(source_length);
	}

	constexpr GLfloat compute_source_sample_point_to_probe_grid_point_bias
	(
		GLuint source_length
	)
	{
		return 0.5 / static_cast<GLfloat>(source_length);
	}

	constexpr GLfloat compute_probe_grid_point_to_fluence_sample_point_factor
	(
		GLuint probe_grid_length
	)
	{
		return 1.0 - 1.0 / static_cast<GLfloat>(probe_grid_length);
	}

	constexpr GLfloat compute_probe_grid_point_to_fluence_sample_point_bias
	(
		GLuint probe_grid_length
	)
	{
		return 0.5 / static_cast<GLfloat>(probe_grid_length);
	}

	constexpr GLuint compute_ray_texture_length
	(
		GLuint probe_grid_length, GLuint cascade, GLuint orthogonal_cascade_count, GLuint cascade_power_of_two
	)
	{
		GLuint texture_length
		{
			((probe_grid_length - 2u + cascade_power_of_two) / cascade_power_of_two - 1u)
			* (cascade_power_of_two + 1u)
		};
		texture_length = std::max(texture_length, probe_grid_length * static_cast<GLuint>(cascade < orthogonal_cascade_count));
		return texture_length;
	}

	constexpr GLuint compute_angular_fluence_texture_length
	(
		GLuint directional_cascade_count, GLuint probe_grid_length
	)
	{
		return std::max(1u << directional_cascade_count, probe_grid_length);
	}

	constexpr GLfloat compute_probe_grid_full_step_to_sample_step_factor
	(
		GLuint source_length, GLuint probe_grid_length
	)
	{
		return (1.0 + 1.0 / static_cast<GLfloat>(source_length)) / static_cast<GLfloat>(probe_grid_length);
	}

	constexpr GLfloat compute_probe_grid_point_to_sample_point_bias
	(
		GLuint source_length
	)
	{
		// IMPORTANT TODO: Is this correct?
		return 0.5 / static_cast<GLfloat>(source_length);
	}

	constexpr GLfloat compute_probe_grid_full_step_to_sample_step_projection
	(
		GLfloat frustum_unit_z_length, GLfloat probe_grid_full_step_to_sample_step_factor
	)
	{
		return frustum_unit_z_length * probe_grid_full_step_to_sample_step_factor;
	}

	void create_vector
	(
		GLuint(&vector)[2u], GLuint x, GLuint y, GLuint bidirection, GLuint orthogonal_bidirection
	);

	GLuint compute_rays_viewport_size
	(
		GLuint(&size)[2u], GLuint probe_grid_length, GLuint cascade_power_of_two, GLuint cascade, GLuint orthogonal_probe_grid_length,
		GLuint bidirection, GLuint orthogonal_bidirection
	);

	GLuint compute_gather_fluence_from_skycircle_viewport_size
	(
		GLuint(&size)[2u], GLuint probe_grid_length, GLuint cascade_power_of_two, GLuint cascade,
		GLuint bidirection, GLuint orthogonal_bidirection
	);

	GLuint compute_merge_fluence_inner_viewport_size
	(
		GLuint(&size)[2u], GLuint probe_grid_length, GLuint cascade_power_of_two, GLuint cascade, GLuint orthogonal_probe_grid_length,
		GLuint bidirection, GLuint orthogonal_bidirection
	);

	GLuint compute_merge_fluence_outer_viewport_start
	(
		GLuint(&start)[2u], GLuint probe_grid_length, GLuint cascade_power_of_two, GLuint cascade,
		GLuint bidirection, GLuint orthogonal_bidirection
	);

	GLuint compute_merge_fluence_outer_viewport_size
	(
		GLuint(&size)[2u], GLuint cascade_power_of_two,
		GLuint bidirection, GLuint orthogonal_bidirection
	);

	void enable(::game_environment::Environment& environment);

	void disable(::game_environment::Environment& environment);

	void toggle(::game_environment::Environment& environment);
}
