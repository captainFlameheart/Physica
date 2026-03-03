#pragma once
#include "fixed_data/include.h"
#include "float_data/include.h"
#include "uint_data/include.h"
#include "uvec4_data/include.h"
#include "key_event/include.h"

namespace game_state::layouts
{
	struct State
	{
		fixed_data::State fixed_data;
		uvec4_data::Aliased_State uvec4_data;
		uint_data::Aliased_State uint_data;
		float_data::Aliased_State float_data;
		key_event::Aliased_State key_event;
	};
}
