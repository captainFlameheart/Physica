#pragma once
#include "warm_start/include.h"
#include "solve/include.h"
#include "integrate_velocities/include.h"
#include "persist/include.h"

namespace game_state::shaders
{
	struct Shader_Set
	{
		warm_start::Shader_Set warm_start;
		solve::Shader_Set solve;
		integrate_velocities::Shader_Set integrate_velocities;
		persist::Shader_Set persist;
	};
}
