#include "RUN_LEGACY.h"
#if RUN_LEGACY == 1
#include "game_logic/util/camera/const_camera_z.h"
#include "game_logic/util/camera/const_camera.h"

namespace game_logic::util::camera
{
	GLfloat camera_z(game_environment::Environment const& environment)
	{
		return camera(environment).z;
	}
}
#endif
