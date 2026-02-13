#include "RUN_LEGACY.h"
#if RUN_LEGACY == 1
#include "game_logic/util/camera/unit_z_y_to_camera_local_world_y.h"
#include "game_logic/util/camera/unit_z_y_to_z_y.h"
#include "game_logic/util/camera/const_camera_z.h"

namespace game_logic::util::camera
{
	void unit_z_y_to_camera_local_world_y
	(
		game_environment::Environment const& environment,
		GLfloat const unit_z_y,
		GLfloat* camera_local_world_y
	)
	{
		return unit_z_y_to_z_y
		(
			unit_z_y, camera_z(environment),
			camera_local_world_y
		);
	}
}
#endif
