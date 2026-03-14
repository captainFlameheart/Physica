#pragma once
#include "glad_glfw.h"

namespace game_state::entity_type_indices
{
	constexpr GLuint base{ 0u };

	namespace bodies
	{
		constexpr GLuint base{ ::game_state::entity_type_indices::base };

		enum struct Indices : GLuint
		{
			point_mass = base,
			rigid_body,

			end
		};

		constexpr GLuint end{ static_cast<GLuint>(Indices::end) };
		constexpr GLuint count{ end - base };
	}

	namespace body_attachments
	{
		constexpr GLuint base{ ::game_state::entity_type_indices::bodies::end };

		enum struct Indices : GLuint
		{
			rigid_body_triangle = base,
			rigid_body_circle,

			end
		};

		constexpr GLuint end{ static_cast<GLuint>(Indices::end) };
		constexpr GLuint count{ end - base };
	}

	namespace constraint_spawners
	{
		constexpr GLuint base{ ::game_state::entity_type_indices::body_attachments::end };

		enum struct Indices : GLuint
		{
			rigid_body_circle_contact_constraint_spawner = base,

			end
		};

		constexpr GLuint end{ static_cast<GLuint>(Indices::end) };
		constexpr GLuint count{ end - base };
	}

	namespace constraints
	{
		constexpr GLuint base{ ::game_state::entity_type_indices::constraint_spawners::end };

		enum struct Indices : GLuint
		{
			point_mass_distance_constraint = base,
			rigid_body_circle_contact_constraint,

			end
		};

		constexpr GLuint end{ static_cast<GLuint>(Indices::end) };
		constexpr GLuint count{ end - base };
	}

	constexpr GLuint end{ static_cast<GLuint>(constraints::end) };
	constexpr GLuint count{ end - base };

	/*enum struct Indices : GLuint
	{
		point_mass,
		rigid_body,
		rigid_body_triangle,
		rigid_body_circle,

		rigid_body_circle_contact_constraint_spawner,

		point_mass_distance_constraint,
		point_mass_uniform_force_constraint,
		rigid_body_circle_contact_constraint,

		count
	};

	constexpr GLuint count{ static_cast<GLuint>(Indices::count) };*/
}
