#include "game_logic/util/camera/camera_xy.h"
#include "game_logic/util/camera/camera.h"

namespace game_logic::util::camera
{
	::util::math::Vector_2D& camera_xy(game_environment::Environment& environment)
	{
		return camera(environment).xy;
	}
}
