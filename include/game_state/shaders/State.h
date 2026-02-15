#pragma once
#include "glad_glfw.h"
#include "draw/include.h"
#include "tick/include.h"

namespace game_state::shaders
{
	struct State
	{
		initialize::Aliased_State initialize;
		tick::Aliased_State tick;
		draw::Aliased_State draw;
	};
}
