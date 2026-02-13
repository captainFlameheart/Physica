#include "RUN_LEGACY.h"
#if RUN_LEGACY == 1
#include "game_logic/util/camera/const_camera.h"

namespace game_logic::util::camera
{
	::game_state::camera::Camera const& camera(game_environment::Environment const& environment)
	{
		return environment.state.camera;
	}
}
#endif
