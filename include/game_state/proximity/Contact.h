#pragma once
#include "glad_glfw.h"
#include "Bounding_Box.h"

namespace game_state::proximity
{
	struct Neighbor_Pair
	{
		GLuint previous;
		GLuint next;
	};

	struct Contact
	{
		union 
		{
			GLuint leafs[2];
			struct
			{
				GLuint leaf_0;
				GLuint leaf_1;
			};
		};
		union
		{
			Neighbor_Pair neighbor_pairs[2];
			struct
			{
				GLuint child_0_neighbor_pair;
				GLuint child_1_neighbor_pair;
			};
		};
	};
}
