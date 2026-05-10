#pragma once
#include "glad_glfw.h"
#include "game_environment/Environment.h"
#include "game_state/profiling/include.h"
#include <iostream>

namespace game_logic::profiling
{
	void fetch_timestamps(::game_environment::Environment& environment)
	{
		if (environment.state.profiling.time_measurement == nullptr)
		{
			return;
		}

		glMemoryBarrier(GL_ALL_BARRIER_BITS);

		GLuint timestamps_size{ environment.state.profiling.time_measurement->timestamp_capacity * ::game_state::profiling::timestamp_value_size };
		GLbyte* timestamps{ new GLbyte[timestamps_size] };
		
		glGetNamedBufferSubData(environment.state.profiling.time_measurement->buffer, 0, timestamps_size, timestamps);

		for (GLuint i{ 0u }; i < environment.state.profiling.time_measurement->timestamp_capacity; ++i)
		{
			GLuint64 timestamp;
			std::memcpy
			(
				&timestamp,
				timestamps + i * ::game_state::profiling::timestamp_value_size,
				::game_state::profiling::timestamp_value_size
			);
			std::cout << timestamp << std::endl;
		}
		
		delete[] timestamps;
	}
}
