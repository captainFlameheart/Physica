#pragma once
#include "glad_glfw.h"
#include <algorithm>

namespace game_logic::holographic_radiance_cascades
{
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
}
