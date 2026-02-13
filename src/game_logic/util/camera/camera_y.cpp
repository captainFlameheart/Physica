#pragma once
#include "RUN_LEGACY.h"
#if RUN_LEGACY == 1
#include "game_logic/util/camera/camera_y.h"
#include "game_logic/util/camera/camera_xy.h"

namespace game_logic::util::camera
{
	GLint& camera_y(game_environment::Environment& environment)
	{
		return camera_xy(environment).y;
	}
}
#endif
