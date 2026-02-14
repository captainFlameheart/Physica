#pragma once
#include "glad_glfw.h"
#include "point_masses/include.h"

namespace game_state::shaders::draw::bodies
{
	struct State
	{
		point_masses::Aliased_State point_masses;
		// TODO: Rigid bodies
	};
}
