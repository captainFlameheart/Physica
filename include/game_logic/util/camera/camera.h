#pragma once
#include "game_environment/environment.h"

namespace game_logic::util::camera
{
	::game_state::camera::Camera& camera(game_environment::Environment& environment);
}
