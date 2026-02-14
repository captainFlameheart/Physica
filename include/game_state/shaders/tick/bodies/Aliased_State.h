#pragma once
#include "glad_glfw.h"
#include "State.h"

namespace game_state::shaders::tick::bodies
{
	union Aliased_State
	{
		State state;
		GLuint shaders[sizeof(State) / sizeof(GLuint)];
	};
}
