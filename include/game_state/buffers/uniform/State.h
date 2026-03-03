#pragma once
#include "glad_glfw.h"

namespace game_state::buffers::uniform
{
	constexpr GLuint key_event_capacity{ 3u };

	struct State
	{
		GLuint buffer;
		GLuint key_event_offsets[key_event_capacity];
	};
}
