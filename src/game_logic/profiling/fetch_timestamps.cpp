#pragma once
#include "glad_glfw.h"
#include "game_environment/Environment.h"
#include "game_state/profiling/include.h"
#include <iostream>
#include "global/include.h"

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
			::game_state::profiling::Timestamp_Metadata& metadata{ environment.state.profiling.time_measurement->timestamp_metadata[i] };
			if (metadata.name != "")
			{
				std::cout << i << ", " << metadata.name << ": " << timestamp << '\n';
				if (metadata.previous != null_uint)
				{
					GLuint64 previous_timestamp;
					std::memcpy
					(
						&previous_timestamp,
						timestamps + metadata.previous * ::game_state::profiling::timestamp_value_size,
						::game_state::profiling::timestamp_value_size
					);

					GLuint64 elapsed_time = timestamp - previous_timestamp;
					std::cout << "	" << elapsed_time << " ns = " << elapsed_time * (1.0 / 1000000.0) << " ms.\n";
				}
			}
		}
		std::cout << std::endl;
		
		delete[] timestamps;
	}
}
