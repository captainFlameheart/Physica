#pragma once
#include "RUN_LEGACY.h"
#if RUN_LEGACY == 1
#include "game_logic/util/camera/camera_x.h"
#include "game_logic/util/camera/camera_xy.h"

namespace game_logic::util::camera
{
	GLint& camera_x(game_environment::Environment& environment)
	{
		return camera_xy(environment).x;
	}
}
#endif
