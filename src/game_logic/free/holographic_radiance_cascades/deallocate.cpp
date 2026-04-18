#pragma once
#include "game_logic/free/holographic_radiance_cascades/include.h"
#include <iostream>

namespace game_logic::free::holographic_radiance_cascades
{
	void deallocate(game_environment::Environment& environment)
	{
		std::cout << "Deallocate holographic radiance cascades." << std::endl;

		glDeleteFramebuffers(1u, &environment.state.holographic_radiance_cascades.source_framebuffer);
		glDeleteTextures(1u, &environment.state.holographic_radiance_cascades.source_texture);

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
	}
}
