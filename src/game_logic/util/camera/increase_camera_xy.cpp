#include "game_logic/util/camera/increase_camera_xy.h"
#include "game_logic/util/camera/camera_x.h"
#include "game_logic/util/camera/camera_y.h"

namespace game_logic::util::camera
{
	void increase_camera_xy
	(
		game_environment::Environment& environment,
		GLint const x_increase, GLint const y_increase
	)
	{
		// TODO: Implement and call separate functions for x and y
		camera_x(environment) += x_increase;
		camera_y(environment) += y_increase;
	}
}
