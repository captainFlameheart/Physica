#include "game_logic/initialize/holographic_radiance_cascades/include.h"
#include "glad_glfw.h"
#include "game_environment/Environment.h"
#include <array>

namespace game_logic::initialize::holographic_radiance_cascades
{
	void allocate_source_image(game_environment::Environment& environment)
	{
		glCreateTextures(GL_TEXTURE_2D_ARRAY, 1u, &environment.state.holographic_radiance_cascades.source_texture);
		glCreateFramebuffers(1u, &environment.state.holographic_radiance_cascades.source_framebuffer);

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

		GLenum source_framebuffer_status{ glCheckNamedFramebufferStatus(environment.state.holographic_radiance_cascades.source_framebuffer, GL_DRAW_FRAMEBUFFER) };
		if (source_framebuffer_status != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cerr << "Holographic Radiance Cascades source framebuffer invalid, status: " << source_framebuffer_status << std::endl;
		}

		glBindTextures(::game_state::texture_units::source_image, 1u, &environment.state.holographic_radiance_cascades.source_texture);
	}

	void allocate_rays(game_environment::Environment& environment)
	{
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

			GLuint ray_draw_buffers[::game_state::holographic_radiance_cascades::ray_layers::count];
			for (GLuint layer{ 0u }; layer < ::game_state::holographic_radiance_cascades::ray_layers::count; ++layer)
			{
				GLenum color_attachment{ GL_COLOR_ATTACHMENT0 + layer };
				glNamedFramebufferTextureLayer
				(
					environment.state.holographic_radiance_cascades.ray_framebuffers[cascade],
					color_attachment,
					environment.state.holographic_radiance_cascades.ray_textures[cascade], 0, layer
				);
				ray_draw_buffers[layer] = color_attachment;
			}
			glNamedFramebufferDrawBuffers(environment.state.holographic_radiance_cascades.ray_framebuffers[cascade], std::size(ray_draw_buffers), ray_draw_buffers);

			GLenum ray_framebuffer_status{ glCheckNamedFramebufferStatus(environment.state.holographic_radiance_cascades.ray_framebuffers[cascade], GL_DRAW_FRAMEBUFFER) };
			if (ray_framebuffer_status != GL_FRAMEBUFFER_COMPLETE)
			{
				std::cerr << "Holographic Radiance Cascades ray framebuffer " << cascade << " invalid, status: " << ray_framebuffer_status << std::endl;
			}
		}
	}

	void allocate_angular_fluence(game_environment::Environment& environment)
	{
		glCreateTextures(GL_TEXTURE_2D_ARRAY, 1u, &environment.state.holographic_radiance_cascades.angular_fluence_texture);
		glCreateFramebuffers(1u, &environment.state.holographic_radiance_cascades.angular_fluence_framebuffer);

		GLuint angular_fluence_texture_width
		{
			::game_logic::holographic_radiance_cascades::compute_angular_fluence_texture_length
			(
				environment.state.holographic_radiance_cascades.horizontal_cascade_count,
				environment.state.holographic_radiance_cascades.probe_grid_width
			)
		};
		GLuint angular_fluence_texture_height
		{
			::game_logic::holographic_radiance_cascades::compute_angular_fluence_texture_length
			(
				environment.state.holographic_radiance_cascades.vertical_cascade_count,
				environment.state.holographic_radiance_cascades.probe_grid_height
			)
		};

		glTextureStorage3D
		(
			environment.state.holographic_radiance_cascades.angular_fluence_texture, 1u, GL_RGBA32F,
			angular_fluence_texture_width, angular_fluence_texture_height,
			2u
		);

		for (GLuint layer{ 0u }; layer < 2u; ++layer)
		{
			GLenum color_attachment{ GL_COLOR_ATTACHMENT0 + layer };
			glNamedFramebufferTextureLayer
			(
				environment.state.holographic_radiance_cascades.angular_fluence_framebuffer,
				color_attachment,
				environment.state.holographic_radiance_cascades.angular_fluence_texture, 0, layer
			);
		}

		GLenum angular_fluence_framebuffer_status{ glCheckNamedFramebufferStatus(environment.state.holographic_radiance_cascades.angular_fluence_framebuffer, GL_DRAW_FRAMEBUFFER) };
		if (angular_fluence_framebuffer_status != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cerr << "Holographic Radiance Cascades angular fluence framebuffer invalid, status: " << angular_fluence_framebuffer_status << std::endl;
		}

		glBindTextures(::game_state::texture_units::upper_cascade_fluence, 1u, &environment.state.holographic_radiance_cascades.angular_fluence_texture);
	}

	void allocate_fluence(game_environment::Environment& environment)
	{
		glCreateTextures(GL_TEXTURE_2D, 1u, &environment.state.holographic_radiance_cascades.fluence_texture);
		glCreateFramebuffers(1u, &environment.state.holographic_radiance_cascades.fluence_framebuffer);

		GLuint fluence_texture_width{ environment.state.holographic_radiance_cascades.probe_grid_width };
		GLuint fluence_texture_height{ environment.state.holographic_radiance_cascades.probe_grid_height };

		glTextureStorage2D
		(
			environment.state.holographic_radiance_cascades.fluence_texture, 1u, GL_RGBA32F,
			fluence_texture_width, fluence_texture_height
		);

		glTextureParameteri
		(
			environment.state.holographic_radiance_cascades.fluence_texture,
			GL_TEXTURE_MIN_FILTER, GL_LINEAR
		);
		glTextureParameteri
		(
			environment.state.holographic_radiance_cascades.fluence_texture,
			GL_TEXTURE_MAG_FILTER, GL_LINEAR
		);

		GLfloat border_color[4u]{ 0.0f, 0.0f, 0.0f, 0.0f };
		glTextureParameterfv
		(
			environment.state.holographic_radiance_cascades.fluence_texture,
			GL_TEXTURE_BORDER_COLOR, border_color
		);

		glTextureParameteri
		(
			environment.state.holographic_radiance_cascades.fluence_texture,
			GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER
		);
		glTextureParameteri
		(
			environment.state.holographic_radiance_cascades.fluence_texture,
			GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER
		);

		glNamedFramebufferTexture
		(
			environment.state.holographic_radiance_cascades.fluence_framebuffer,
			GL_COLOR_ATTACHMENT0,
			environment.state.holographic_radiance_cascades.fluence_texture, 0
		);
		glNamedFramebufferDrawBuffer
		(
			environment.state.holographic_radiance_cascades.fluence_framebuffer,
			GL_COLOR_ATTACHMENT0
		);

		GLenum fluence_framebuffer_status{ glCheckNamedFramebufferStatus(environment.state.holographic_radiance_cascades.fluence_framebuffer, GL_DRAW_FRAMEBUFFER) };
		if (fluence_framebuffer_status != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cerr << "Holographic Radiance Cascades fluence framebuffer invalid, status: " << fluence_framebuffer_status << std::endl;
		}

		glBindTextures(::game_state::texture_units::fluence, 1u, &environment.state.holographic_radiance_cascades.fluence_texture);
	}

	void allocate(game_environment::Environment& environment)
	{
		std::cout << "Allocate Holographic Radiance Cascades." << std::endl;

		allocate_source_image(environment);
		allocate_rays(environment);
		allocate_angular_fluence(environment);
		allocate_fluence(environment);
	}
}
