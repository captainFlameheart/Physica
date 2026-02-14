#pragma once
#include "glad_glfw.h"
#include "draw/include.h"

namespace game_state::shaders
{
	struct State
	{
		// TODO: Tick
		draw::Aliased_State draw;
	};
}
