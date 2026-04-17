#include "glad_glfw.h"
#include "game_environment/Environment.h"
#include "game_logic/initialize/holographic_radiance_cascades/include.h"

namespace game_logic::initialize::holographic_radiance_cascades
{
	void initialize(game_environment::Environment& environment)
	{
		glCreateTextures(GL_TEXTURE_2D_ARRAY, 1u, &environment.state.holographic_radiance_cascades.source_texture);
		// TODO: Allow for different internal formats.
		glTextureStorage2D
		(
			environment.state.holographic_radiance_cascades.source_texture, ::game_state::holographic_radiance_cascades::source_layers::count,
			GL_RGBA32F, environment.state.glfw.framebuffer_width, environment.state.glfw.framebuffer_height
		);
	}
}
