#include "game_logic/profiling/include.h"
#include "glad_glfw.h"
#include "game_environment/Environment.h"
#include "game_state/profiling/include.h"

namespace game_logic::profiling
{
	void disable_timing(::game_environment::Environment& environment, GLuint type)
	{
		if (environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)] != nullptr)
		{
			std::cout << "Disable timing of type " << ::game_state::profiling::timing_type_names[static_cast<GLuint>(type)] << std::endl;

			delete[] (environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->metadata);
			delete[] (environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->generation_start_timestamps);
			
			glDeleteQueries
			(
				environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->query_capacity,
				environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->queries
			);
			glDeleteQueries
			(
				environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->generation_query_capacity,
				environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->generation_queries
			);

			glDeleteBuffers
			(
				2u,
				environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->buffers
			);

			environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)] = nullptr;
		}
	}
}
