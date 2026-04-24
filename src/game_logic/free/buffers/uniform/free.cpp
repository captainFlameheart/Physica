#pragma once
#include "game_logic/free/buffers/uniform/include.h"
#include <iostream>

namespace game_logic::free::buffers::uniform
{
	void free(game_environment::Environment& environment)
	{
		std::cout << "Free uniform buffer" << std::endl;
		glDeleteBuffers(1u, &environment.state.buffers.uniform.buffer);
		for (GLuint i{ 0u }; i < ::game_state::buffers::uniform::key_event_capacity; ++i)
		{
			glDeleteSync(environment.state.buffers.uniform.key_event_fences[i]);
		}
		for (GLuint i{ 0u }; i < ::game_state::buffers::uniform::mouse_button_event_capacity; ++i)
		{
			glDeleteSync(environment.state.buffers.uniform.mouse_button_event_fences[i]);
		}
		for (GLuint i{ 0u }; i < ::game_state::buffers::uniform::on_events_polled_capacity; ++i)
		{
			glDeleteSync(environment.state.buffers.uniform.on_events_polled_fences[i]);
		}
	}
}
