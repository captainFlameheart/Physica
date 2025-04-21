#pragma once
#include "glad_glfw.h"
#include "Bounding_Box.h"

namespace game_state::proximity
{
	struct Node
	{
		Bounding_Box bounding_box;
		union
		{
			GLuint parent;
			GLuint next_free;
		};
		union
		{
			GLuint children[2];
			struct
			{
				GLuint contact;
				GLuint change;
			};
		};
	};
}
