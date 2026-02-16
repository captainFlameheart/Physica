#pragma once
#include "glad_glfw.h"

namespace game_state::entity_type_indices
{
	enum struct Indices : GLuint
	{
		point_mass,
		rigid_body,
		point_mass_distance_constraint,
		point_mass_uniform_force_constraint,

		count
	};

	constexpr GLuint count{ static_cast<GLuint>(Indices::count) };
}
