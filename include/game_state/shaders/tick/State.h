#pragma once
#include "bodies/include.h"
#include "glad_glfw.h"

namespace game_state::shaders::tick
{
	struct State
	{
		bodies::Aliased_State bodies;
		// TODO: Constraints
	};
}
