#pragma once
#include "glad_glfw.h"

namespace game_state::holographic_radiance_cascades::configuration
{
	struct Offset_Pair
	{
		GLuint merge_rays;
		GLuint merge_fluence;
	};

	struct State
	{
		GLuint buffer;

		union
		{
			struct
			{
				union
				{
					struct
					{
						Offset_Pair east_offset_pair;
						Offset_Pair west_offset_pair;
					};
					Offset_Pair horizontal_offset_pairs[2u];
				};
				union
				{
					struct
					{
						Offset_Pair north_offset_pair;
						Offset_Pair south_offset_pair;
					};
					Offset_Pair vertical_offset_pairs[2u];
				};
			};
			Offset_Pair offset_pairs[2u][2u];
		};

		GLuint merge_rays_stride;
		GLuint merge_fluence_stride;
	};
}
