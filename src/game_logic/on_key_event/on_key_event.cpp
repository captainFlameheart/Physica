#include "game_logic/on_key_event/include.h"
#include "game_environment/environment.h"

namespace game_logic::on_key_event
{
	void on_key_event(
		game_environment::Environment& environment,
		int const key,
		int const scancode,
		int const action,
		int const mods
	)
	{
		std::cout << "Key event" << std::endl;
		glUseProgram(environment.state.shaders[static_cast<GLuint>(::game_state::shader_indices::on_key_event::Indices::on_key_event)]);
		glDispatchCompute(1u, 1u, 1u);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	}
}
