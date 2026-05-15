#include "game_logic/profiling/include.h"
#include "glad_glfw.h"
#include "game_environment/Environment.h"
#include "global/include.h"

namespace game_logic::profiling
{
	void start_next_generation(::game_environment::Environment& environment, ::game_state::profiling::Timestamp_Type type)
	{
		if (environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)] == nullptr)
		{
			return;
		}
		if (environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->generation_query_count == environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->generation_query_capacity)
		{
			std::cerr << "Timestamp generation query capacity exceeded!" << std::endl;
			return;
		}

		GLuint generation_query_index{ environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->generation_query_count++ };
		glQueryCounter(environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->generation_queries[generation_query_index], GL_TIMESTAMP);

		++environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->next_generation;
	}
}
