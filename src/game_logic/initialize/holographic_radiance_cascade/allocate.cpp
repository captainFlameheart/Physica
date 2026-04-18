#include "game_logic/initialize/holographic_radiance_cascades/include.h"
#include "glad_glfw.h"
#include "game_environment/Environment.h"
#include <array>

namespace game_logic::initialize::holographic_radiance_cascades
{
	void allocate(game_environment::Environment& environment)
	{
		std::cout << "Allocate Holographic Radiance Cascades." << std::endl;

		glCreateTextures(GL_TEXTURE_2D_ARRAY, 1u, &environment.state.holographic_radiance_cascades.source_texture);
		// TODO: Allow for different internal formats.
		glTextureStorage3D
		(
			environment.state.holographic_radiance_cascades.source_texture, 1u, GL_RGBA32F,
			environment.state.glfw.framebuffer_width, environment.state.glfw.framebuffer_height,
			::game_state::holographic_radiance_cascades::source_layers::count
		);
		
		glTextureParameteri
		(
			environment.state.holographic_radiance_cascades.source_texture,
			GL_TEXTURE_MIN_FILTER, GL_LINEAR
		);
		glTextureParameteri
		(
			environment.state.holographic_radiance_cascades.source_texture,
			GL_TEXTURE_MAG_FILTER, GL_LINEAR
		);

		GLfloat border_color[4u]{ 0.0f, 0.0f, 0.0f, 0.0f };
		glTextureParameterfv
		(
			environment.state.holographic_radiance_cascades.source_texture,
			GL_TEXTURE_BORDER_COLOR, border_color
		);

		glTextureParameteri
		(
			environment.state.holographic_radiance_cascades.source_texture,
			GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER
		);
		glTextureParameteri
		(
			environment.state.holographic_radiance_cascades.source_texture,
			GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER
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

		environment.state.holographic_radiance_cascades.ray_textures = new GLuint[environment.state.holographic_radiance_cascades.cascade_count];
		environment.state.holographic_radiance_cascades.ray_framebuffers = new GLuint[environment.state.holographic_radiance_cascades.cascade_count];

		glCreateTextures
		(
			GL_TEXTURE_2D_ARRAY,
			environment.state.holographic_radiance_cascades.cascade_count,
			environment.state.holographic_radiance_cascades.ray_textures
		);
		glCreateFramebuffers
		(
			environment.state.holographic_radiance_cascades.cascade_count,
			environment.state.holographic_radiance_cascades.ray_framebuffers
		);

		for (GLuint cascade{ 0u }; cascade < environment.state.holographic_radiance_cascades.cascade_count; ++cascade)
		{
			GLuint cascade_power_of_two{ 1u << cascade };
			GLuint ray_texture_width
			{
				::game_logic::holographic_radiance_cascades::compute_ray_texture_length
				(
					environment.state.holographic_radiance_cascades.probe_grid_width,
					cascade,
					environment.state.holographic_radiance_cascades.vertical_cascade_count,
					cascade_power_of_two
				)
			};
			GLuint ray_texture_height
			{
				::game_logic::holographic_radiance_cascades::compute_ray_texture_length
				(
					environment.state.holographic_radiance_cascades.probe_grid_height,
					cascade,
					environment.state.holographic_radiance_cascades.horizontal_cascade_count,
					cascade_power_of_two
				)
			};
			// TODO: Allow radiance and transmittance layers to be merged.
			glTextureStorage3D
			(
				environment.state.holographic_radiance_cascades.ray_textures[cascade], 1u, GL_RGBA32F,
				ray_texture_width, ray_texture_height,
				::game_state::holographic_radiance_cascades::ray_layers::count
			);
		}
	}
}
