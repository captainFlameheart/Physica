#pragma once
#include "glad_glfw.h"

namespace game_state::buffers::uniform
{
	constexpr GLuint key_event_capacity{ 4u };
	constexpr GLuint mouse_button_event_capacity{ 4u };
	constexpr GLuint on_events_polled_capacity{ 4u };

	struct State
	{
		GLuint buffer;
		GLbyte* mapping;
		
		GLuint key_event_offset;
		GLsync key_event_fences[key_event_capacity];
		GLuint key_event_index;

		GLuint mouse_button_event_offset;
		GLsync mouse_button_event_fences[mouse_button_event_capacity];
		GLuint mouse_button_event_index;

		GLuint on_events_polled_offset;
		GLsync on_events_polled_fences[on_events_polled_capacity];
		GLuint on_events_polled_index;
	};
}
