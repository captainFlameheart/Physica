#include "RUN_LEGACY.h"
#if RUN_LEGACY == 1
#pragma once
#include "game_environment/environment.h"
#include "game_state/camera/camera.h"

namespace game_logic::util::camera
{
	::game_state::camera::Camera& camera(game_environment::Environment& environment);
}
#endif
