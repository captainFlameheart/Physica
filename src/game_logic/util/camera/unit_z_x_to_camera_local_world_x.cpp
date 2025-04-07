#include "game_logic/util/camera/unit_z_x_to_camera_local_world_x.h"
#include "game_logic/util/camera/unit_z_x_to_z_x.h"
#include "game_logic/util/camera/const_camera_z.h"

namespace game_logic::util::camera
{
	void unit_z_x_to_camera_local_world_x
	(
		game_environment::Environment const& environment,
		GLfloat const unit_z_x,
		GLfloat* camera_local_world_x
	)
	{
		return unit_z_x_to_z_x
		(
			unit_z_x, camera_z(environment),
			camera_local_world_x
		);
	}
}
