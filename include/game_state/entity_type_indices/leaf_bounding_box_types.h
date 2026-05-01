#pragma once
#include "glad_glfw.h"

namespace game_state::leaf_bounding_box_types
{
	constexpr GLuint base{ 0u };

	enum struct Indices : GLuint
	{
		rigid_body_triangle = base,
		rigid_body_circle,

		end
	};

	constexpr GLuint end{ static_cast<GLuint>(Indices::end) };
	constexpr GLuint count{ end - base };
	constexpr GLuint contact_type_count{ (count * (count + 1u)) >> 1u };
}
