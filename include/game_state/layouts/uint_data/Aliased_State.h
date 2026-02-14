#pragma once
#include "State.h"

namespace game_state::layouts::uint_data
{
	union Aliased_State
	{
		State state;
		GLint properties[sizeof(State) / sizeof(GLint)];
	};
}
