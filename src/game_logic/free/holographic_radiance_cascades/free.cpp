#pragma once
#include "game_logic/free/holographic_radiance_cascades/include.h"
#include <iostream>

namespace game_logic::free::holographic_radiance_cascades
{
	void free(game_environment::Environment& environment)
	{
		std::cout << "Free holographic radiance cascades." << std::endl;

		glDeleteFramebuffers(1u, &environment.state.holographic_radiance_cascades.source_framebuffer);
		glDeleteTextures(1u, &environment.state.holographic_radiance_cascades.source_texture);
	}
}
