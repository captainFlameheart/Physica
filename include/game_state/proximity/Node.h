#pragma once
#include "glad_glfw.h"
#include "Bounding_Box.h"

namespace game_state::proximity
{
	struct Node
	{
		Bounding_Box bounding_box;	// 16 bytes
		union
		{
			GLuint parent;
			GLuint next_free;
		};	// 4 bytes
		union
		{
			union
			{
				GLuint children[2];
				struct
				{
					GLuint child_0;
					GLuint child_1;
				};
			};
			struct
			{
				GLuint contact;
				GLuint change;
			};
		};	// 8 bytes
	};	// 28 bytes
}
