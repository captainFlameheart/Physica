#pragma once

namespace game_state::holographic_radiance_cascades
{
	struct State
	{
		bool enabled;
		GLuint visible_source_layer;

		union
		{
			struct
			{
				GLuint probe_grid_width;
				GLuint probe_grid_height;
			};
			GLuint probe_grid_size[2u];
		};
		union
		{
			struct
			{
				GLuint horizontal_cascade_count;
				GLuint vertical_cascade_count;
			};
			GLuint cascade_counts[2u];
		};
		GLuint cascade_count;

		GLuint source_texture;
		GLuint source_framebuffer;

		GLuint* ray_textures;
		GLuint* ray_framebuffers;

		GLuint angular_fluence_texture;
		GLuint angular_fluence_framebuffer;
	};
}
