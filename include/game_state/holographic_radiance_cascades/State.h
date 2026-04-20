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
				GLuint source_width;
				GLuint source_height;
			};
			GLuint source_size[2u];
		};

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

		GLuint skycircle_length;
		union
		{
			struct
			{
				GLuint horizontal_skycircle_sample_count;
				GLuint vertical_skycircle_sample_count;
			};
			GLuint skycircle_sample_counts[2u];
		};

		union
		{
			struct
			{
				GLuint horizontal_trace_rays_cascade_count;
				GLuint vertical_trace_rays_cascade_count;
			};
			GLuint trace_rays_cascade_counts[2u];
		};
		union
		{
			struct
			{
				GLuint horizontal_trace_rays_base_sample_count;
				GLuint vertical_trace_rays_base_sample_count;
			};
			GLuint trace_rays_base_sample_counts[2u];
		};

		union
		{
			struct
			{
				GLuint horizontal_merge_rays_cascade_count;
				GLuint vertical_merge_rays_cascade_count;
			};
			GLuint merge_rays_cascade_counts[2u];
		};

		union
		{
			struct
			{
				GLuint horizontal_skycircle_sample_count;
				GLuint vertical_skycircle_sample_count;
			};
			GLuint skycircle_sample_counts[2u];
		};

		GLuint source_texture;
		GLuint source_framebuffer;

		GLuint skycircle_texture;
		GLuint skycircle_framebuffer;

		GLuint* ray_textures;
		GLuint* ray_framebuffers;

		GLuint angular_fluence_texture;
		GLuint angular_fluence_framebuffer;

		GLuint fluence_texture;
		GLuint fluence_framebuffer;

		union
		{
			struct
			{
				union
				{
					struct
					{
						GLuint* east_trace_rays_shaders;
						GLuint* west_trace_rays_shaders;
					};
					GLuint* horizontal_trace_rays_shaders[2u];
				};
				union
				{
					struct
					{
						GLuint* north_trace_rays_shaders;
						GLuint* south_trace_rays_shaders;
					};
					GLuint* vertical_trace_rays_shaders[2u];
				};
			};
			GLuint* trace_rays_shaders[2u][2u];
		};

		configuration::State configuration;
	};
}
