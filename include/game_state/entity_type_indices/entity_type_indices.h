#pragma once
#include "glad_glfw.h"
#include "game_state/entity_type_indices/leaf_bounding_box_types.h"
#include <array>
#include <string_view>
#include <algorithm>
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
			//rigid_body_circle_contact_constraint_spawner,

			end
		};

		constexpr GLuint end{ static_cast<GLuint>(Indices::end) };
		constexpr GLuint count{ end - base };
		constexpr GLuint total_count{ count + ::game_state::leaf_bounding_box_types::contact_type_count };
	}

	namespace constraints
	{
		constexpr GLuint base{ ::game_state::entity_type_indices::constraint_spawners::base + 1u + ::game_state::leaf_bounding_box_types::contact_type_count };

		enum struct Indices : GLuint
		{
			point_mass_distance_constraint = base,
			rigid_body_triangle_contact_constraint,
			rigid_body_triangle_circle_contact_constraint,
			rigid_body_circle_contact_constraint,

			end
		};

		constexpr GLuint end{ static_cast<GLuint>(Indices::end) };
		constexpr GLuint count{ end - base };
	}
	
	namespace skycircle_elements
	{
		constexpr GLuint base{ ::game_state::entity_type_indices::constraints::end };

		enum struct Indices : GLuint
		{
			default_skycircle_element = base,

			end
		};

		constexpr GLuint end{ static_cast<GLuint>(Indices::end) };
		constexpr GLuint count{ end - base };
	}

	namespace hover_highlighters
	{
		constexpr GLuint base{ ::game_state::entity_type_indices::skycircle_elements::end };

		enum struct Indices : GLuint
		{
			rigid_body_triangle_hover_constraint = base,
			rigid_body_circle_hover_highlighter,

			end
		};

		constexpr GLuint end{ static_cast<GLuint>(Indices::end) };
		constexpr GLuint count{ end - base };
	}

	constexpr GLuint end{ static_cast<GLuint>(hover_highlighters::end) };
	constexpr GLuint count{ end - base };

	constexpr GLuint get_contact_type(::game_state::leaf_bounding_box_types::Indices leaf_type_0, ::game_state::leaf_bounding_box_types::Indices leaf_type_1)
	{
		GLuint min_leaf_type{ std::min(static_cast<GLuint>(leaf_type_0), static_cast<GLuint>(leaf_type_1)) };
		GLuint max_leaf_type{ std::max(static_cast<GLuint>(leaf_type_0), static_cast<GLuint>(leaf_type_1)) };

		return ((max_leaf_type * (max_leaf_type + 1u)) >> 1u) + min_leaf_type;
	}

	constexpr GLuint rigid_body_triangle_contact{ get_contact_type(::game_state::leaf_bounding_box_types::Indices::rigid_body_triangle, ::game_state::leaf_bounding_box_types::Indices::rigid_body_triangle) };
	constexpr GLuint rigid_body_triangle_circle_contact{ get_contact_type(::game_state::leaf_bounding_box_types::Indices::rigid_body_triangle, ::game_state::leaf_bounding_box_types::Indices::rigid_body_circle) };
	constexpr GLuint rigid_body_circle_contact{ get_contact_type(::game_state::leaf_bounding_box_types::Indices::rigid_body_circle, ::game_state::leaf_bounding_box_types::Indices::rigid_body_circle) };

	constexpr GLuint get_contact_detector_type(::game_state::leaf_bounding_box_types::Indices leaf_type_0, ::game_state::leaf_bounding_box_types::Indices leaf_type_1)
	{
		GLuint min_leaf_type{ std::min(static_cast<GLuint>(leaf_type_0), static_cast<GLuint>(leaf_type_1)) };
		GLuint max_leaf_type{ std::max(static_cast<GLuint>(leaf_type_0), static_cast<GLuint>(leaf_type_1)) };

		return constraint_spawners::base + 1u + ((max_leaf_type * (max_leaf_type + 1u)) >> 1u) + min_leaf_type;
	}

	constexpr GLuint rigid_body_triangle_contact_detector{ get_contact_detector_type(::game_state::leaf_bounding_box_types::Indices::rigid_body_triangle, ::game_state::leaf_bounding_box_types::Indices::rigid_body_triangle) };
	constexpr GLuint rigid_body_triangle_circle_contact_detector{ get_contact_detector_type(::game_state::leaf_bounding_box_types::Indices::rigid_body_triangle, ::game_state::leaf_bounding_box_types::Indices::rigid_body_circle) };
	constexpr GLuint rigid_body_circle_contact_detector{ get_contact_detector_type(::game_state::leaf_bounding_box_types::Indices::rigid_body_circle, ::game_state::leaf_bounding_box_types::Indices::rigid_body_circle) };

	constexpr std::array<std::string_view, count> initialize_names()
	{
		std::array<std::string_view, count> names{};

		names[static_cast<GLuint>(constraint_spawners::Indices::bounding_box_contact_detector)] = "bounding_box_contact_detector"sv;

		names[static_cast<GLuint>(constraints::Indices::rigid_body_circle_contact_constraint)] = "rigid_body_circle_contact_constraint"sv;

		return names;
	}

	constexpr std::array<std::string_view, count> names{ initialize_names() };
}
