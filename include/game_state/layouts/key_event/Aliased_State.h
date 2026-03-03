#pragma once
#include "State.h"

namespace game_state::layouts::key_event
{
	union Aliased_State
	{
		State state;
		GLint properties[sizeof(State) / sizeof(GLint)];
	};
}
