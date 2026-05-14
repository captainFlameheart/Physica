#include "game_logic/profiling/include.h"
#include "glad_glfw.h"
#include "game_environment/Environment.h"
#include "global/include.h"

namespace game_logic::profiling
{
	void place_fence(::game_environment::Environment& environment, ::game_state::profiling::Timestamp_Type type)
	{
		if (environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)] == nullptr)
		{
			return;
		}

		GLuint fence_index{ environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->next_metadata_fence_index };
		environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->metadata_fences[fence_index] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
		++environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->next_metadata_fence_index;
	}
}
