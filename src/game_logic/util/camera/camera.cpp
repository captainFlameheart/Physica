#include "game_logic/util/camera/camera.h"

namespace game_logic::util::camera
{
	::game_state::camera::Camera& camera(game_environment::Environment& environment)
	{
		return environment.state.camera;
	}
}
