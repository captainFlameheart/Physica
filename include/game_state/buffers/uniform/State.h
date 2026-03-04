#pragma once
#include "glad_glfw.h"

namespace game_state::buffers::uniform
{
	constexpr GLuint key_event_capacity{ 3u };

	struct State
	{
		GLuint buffer;
		GLbyte* mapping;
		
		GLuint key_event_offset;
		GLsync key_event_fences[key_event_capacity];
		GLuint key_event_index;
	};
}
