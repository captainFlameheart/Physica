#pragma once
#include "warm_start/include.h"
#include "solve/include.h"

namespace game_state::shaders
{
	struct Shader_Set
	{
		warm_start::Shader_Set warm_start;
		solve::Shader_Set solve;
	};
}
