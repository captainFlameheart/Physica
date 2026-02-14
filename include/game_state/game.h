#pragma once
#include "glad_glfw.h"
#include "OpenGL_capabilities/include.h"
#include "shaders/include.h"
#include "layouts/include.h"

namespace game_state
{
	struct Game
	{
		OpenGL_capabilities::State OpenGL_capabilities;
		shaders::Aliased_State shaders;
		layouts::State layouts;
	};
}
