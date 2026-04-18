#pragma once
#include "glad_glfw.h"
#include <array>
#include <string_view>
using namespace std::string_view_literals;

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

	namespace bounding_volume_hierarchy
	{
		constexpr GLuint base{ ::game_state::entity_type_indices::body_attachments::end };

		enum struct Indices : GLuint
		{
			inner_bounding_box = base,

			end
		};

		constexpr GLuint end{ static_cast<GLuint>(Indices::end) };
		constexpr GLuint count{ end - base };
	}

	namespace constraint_spawners	// TODO: Rename to contact detectors.
	{
		constexpr GLuint base{ ::game_state::entity_type_indices::bounding_volume_hierarchy::end };

		enum struct Indices : GLuint
		{
			bounding_box_contact_detector = base,
			rigid_body_circle_contact_constraint_spawner,

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

	constexpr std::array<std::string_view, count> initialize_names()
	{
		std::array<std::string_view, count> names{};
		names[static_cast<GLuint>(constraint_spawners::Indices::bounding_box_contact_detector)] = "bounding_box_contact_detector"sv;
		return names;
	}

	constexpr std::array<std::string_view, count> names{ initialize_names() };
}
