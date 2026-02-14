#pragma once
#include "glad_glfw.h"

namespace game_state::entity_types
{
	enum struct Count_Indices : GLuint
	{
		point_mass,
		point_mass_distance_constraint,
		point_mass_uniform_force_constraint,

		count
	};

	constexpr GLuint count{ static_cast<GLuint>(Count_Indices::count) };
}
