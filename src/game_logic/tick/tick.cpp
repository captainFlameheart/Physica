#include "game_logic/tick/include.h"
#include "game_environment/environment.h"

namespace game_logic::tick
{
	void tick(game_environment::Environment& environment)
	{
		glUseProgram(environment.state.shaders.state.tick.state.bodies.state.point_masses.state.process);
		glDispatchCompute(1u, 1u, 1u);	// TODO: Indirect
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	}
}
