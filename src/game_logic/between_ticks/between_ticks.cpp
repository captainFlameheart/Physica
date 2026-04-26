#include "glad_glfw.h"
#include "game_logic/between_ticks/include.h"
#include "game_environment/environment.h"

namespace game_logic::between_ticks
{
	void between_ticks(game_environment::Environment& environment)
	{
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	}
}
