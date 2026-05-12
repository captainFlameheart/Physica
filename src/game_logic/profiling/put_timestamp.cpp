#include "game_logic/profiling/include.h"
#include "glad_glfw.h"
#include "game_environment/Environment.h"
#include "global/include.h"

namespace game_logic::profiling
{
	void put_timestamp(::game_environment::Environment& environment, ::game_state::profiling::Timestamp_Type type, GLuint generation, std::string name, GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
	{
		if (environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)] == nullptr)
		{
			return;
		}
		if (environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->query_count == environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->query_capacity)
		{
			std::cerr << "Timestamp query capacity exceeded!" << std::endl;
			return;
		}

		GLuint query_index{ environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->query_count++ };
		glQueryCounter(environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->queries[query_index], GL_TIMESTAMP);

		GLuint timestamp_index{ environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->next_timestamp++ };
		environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->metadata[timestamp_index].generation = generation;
		environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->metadata[timestamp_index].color[0u] = red;
		environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->metadata[timestamp_index].color[1u] = green;
		environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->metadata[timestamp_index].color[2u] = blue;
		environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->metadata[timestamp_index].color[3u] = alpha;
		environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->metadata[timestamp_index].name = name;

		// TODO: Write to metadata buffer.
	}
}
