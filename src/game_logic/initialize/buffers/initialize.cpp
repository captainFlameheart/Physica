#include "game_logic/initialize/buffers/include.h"

namespace game_logic::initialize::buffers
{
	void initialize(game_environment::Environment& environment)
	{
		GPU_only::initialize(environment);
	}
}
