#pragma once
#include "uvec4_data/include.h"

namespace game_state::layouts
{
	struct State
	{
		fixed_data::State fixed_data;
		uvec4_data::Aliased_State uvec4_data;
		uint_data::Aliased_State uint_data;
	};
}
