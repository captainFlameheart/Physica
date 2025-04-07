#include "game_logic/util/camera/unit_z_coordinate_to_camera_local_world_coordinate.h"
#include "game_logic/util/camera/unit_z_coordinate_to_z_coordinate.h"
#include "game_logic/util/camera/const_camera_z.h"

namespace game_logic::util::camera
{
	void unit_z_coordinate_to_camera_local_world_coordinate
	(
		game_environment::Environment const& environment,
		GLfloat const unit_z_coordinate,
		GLfloat* camera_local_world_coordinate
	)
	{
		return unit_z_coordinate_to_z_coordinate
		(
			unit_z_coordinate, camera_z(environment), 
			camera_local_world_coordinate
		);
	}
}
