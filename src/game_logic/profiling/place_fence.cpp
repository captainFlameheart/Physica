#include "game_logic/profiling/include.h"
#include "glad_glfw.h"
#include "game_environment/Environment.h"
#include "global/include.h"

namespace game_logic::profiling
{
	void place_fence(::game_environment::Environment& environment, GLuint type)
	{
		if (environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)] == nullptr)
		{
			return;
		}

		GLuint fence_index{ environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->next_metadata_fence_index };
		environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->metadata_fences[fence_index] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
		GLuint next_timestamp{ environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->next_timestamp };
		environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->metadata_fence_timestamps[fence_index] = next_timestamp;
		environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->next_metadata_fence_index = 
			(environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->next_metadata_fence_index + 1u) % ::game_state::profiling::metadata_fence_count;
	}
}
