#pragma once
#include "game_logic/free/holographic_radiance_cascades/include.h"
#include <iostream>

namespace game_logic::free::holographic_radiance_cascades
{
	void deallocate(game_environment::Environment& environment)
	{
		std::cout << "Deallocate holographic radiance cascades." << std::endl;

		for (GLuint bidirection{ 0u }; bidirection < 2u; ++bidirection)
		{
			GLuint trace_rays_shader_count{ environment.state.holographic_radiance_cascades.trace_rays_cascade_counts[bidirection] };
			for (GLuint direction{ 0u }; direction < 2u; ++direction)
			{
				for (GLuint cascade{ 0u }; cascade < trace_rays_shader_count; ++cascade)
				{
					glDeleteProgram(environment.state.holographic_radiance_cascades.trace_rays_shaders[bidirection][direction][cascade]);
				}

				delete[] environment.state.holographic_radiance_cascades.trace_rays_shaders[bidirection][direction];
			}
		}

		glDeleteBuffers(1u, &environment.state.holographic_radiance_cascades.configuration.buffer);

		glDeleteFramebuffers(1u, &environment.state.holographic_radiance_cascades.source_framebuffer);
		glDeleteTextures(1u, &environment.state.holographic_radiance_cascades.source_texture);

		glDeleteFramebuffers(1u, &environment.state.holographic_radiance_cascades.skycircle_framebuffer);
		glDeleteTextures(1u, &environment.state.holographic_radiance_cascades.skycircle_texture);

		glDeleteFramebuffers
		(
			environment.state.holographic_radiance_cascades.cascade_count,
			environment.state.holographic_radiance_cascades.ray_framebuffers
		);
		glDeleteTextures
		(
			environment.state.holographic_radiance_cascades.cascade_count,
			environment.state.holographic_radiance_cascades.ray_textures
		);

		delete[] environment.state.holographic_radiance_cascades.ray_framebuffers;
		delete[] environment.state.holographic_radiance_cascades.ray_textures;

		glDeleteFramebuffers(1u, &environment.state.holographic_radiance_cascades.angular_fluence_framebuffer);
		glDeleteTextures(1u, &environment.state.holographic_radiance_cascades.angular_fluence_texture);

		glDeleteFramebuffers(1u, &environment.state.holographic_radiance_cascades.fluence_framebuffer);
		glDeleteTextures(1u, &environment.state.holographic_radiance_cascades.fluence_texture);
	}
}
