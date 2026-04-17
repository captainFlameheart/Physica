#pragma once

namespace game_state::holographic_radiance_cascades
{
	struct State
	{
		bool enabled;
		GLuint visible_source_layer;

		GLuint source_texture;
		GLuint source_framebuffer;
	};
}
