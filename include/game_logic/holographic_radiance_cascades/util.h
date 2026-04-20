#pragma once
#include "glad_glfw.h"
#include <algorithm>

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
}
