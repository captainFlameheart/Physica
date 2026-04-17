#include "game_logic/initialize/holographic_radiance_cascades/include.h"
#include "glad_glfw.h"
#include "game_environment/Environment.h"
#include <array>

namespace game_logic::initialize::holographic_radiance_cascades
{
	void initialize(game_environment::Environment& environment)
	{
		environment.state.holographic_radiance_cascades.enabled = true;
		environment.state.holographic_radiance_cascades.visible_source_layer = 0u;

		glCreateTextures(GL_TEXTURE_2D_ARRAY, 1u, &environment.state.holographic_radiance_cascades.source_texture);
		// TODO: Allow for different internal formats.
		glTextureStorage3D
		(
			environment.state.holographic_radiance_cascades.source_texture, 1u, GL_RGBA32F,
			environment.state.glfw.framebuffer_width, environment.state.glfw.framebuffer_height,
			::game_state::holographic_radiance_cascades::source_layers::count
		);

		glCreateFramebuffers(1u, &environment.state.holographic_radiance_cascades.source_framebuffer);
		GLenum source_draw_buffers[::game_state::holographic_radiance_cascades::source_layers::count];
		for (GLuint layer{ 0u }; layer < ::game_state::holographic_radiance_cascades::source_layers::count; ++layer)
		{
			GLenum color_attachment{ GL_COLOR_ATTACHMENT0 + layer };
			glNamedFramebufferTextureLayer
			(
				environment.state.holographic_radiance_cascades.source_framebuffer,
				color_attachment,
				environment.state.holographic_radiance_cascades.source_texture, 0, layer
			);
			source_draw_buffers[layer] = color_attachment;
		}
		glNamedFramebufferDrawBuffers(environment.state.holographic_radiance_cascades.source_framebuffer, std::size(source_draw_buffers), source_draw_buffers);

		GLenum source_framebuffer_status{ glCheckFramebufferStatus(GL_FRAMEBUFFER) };
		if (source_framebuffer_status != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cerr << "Holographic Radiance Cascades source framebuffer invalid, status: " << source_framebuffer_status << std::endl;
		}

		glBindTextures(::game_state::texture_units::source_image, 1u, &environment.state.holographic_radiance_cascades.source_texture);
	}
}
