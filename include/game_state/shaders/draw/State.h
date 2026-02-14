#pragma once
#include "glad_glfw.h"
#include "bodies/include.h"

namespace game_state::shaders::draw
{
	struct State
	{
		bodies::Aliased_State bodies;
		// TODO: Constraints
	};
}
