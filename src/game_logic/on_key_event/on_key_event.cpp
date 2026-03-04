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
		
	}
}
