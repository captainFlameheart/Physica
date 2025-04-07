#pragma once
#include "game_logic/util/camera/unit_z_vector_to_camera_local_world_vector.h"
#include "game_logic/util/camera/unit_z_vector_to_z_vector.h"
#include "game_logic/util/camera/const_camera_z.h"

namespace game_logic::util::camera
{
	void unit_z_vector_to_camera_local_world_vector
	(
		game_environment::Environment const& environment,
		GLfloat const unit_z_x, GLfloat const unit_z_y,
		GLfloat* camera_local_world_x, GLfloat* camera_local_world_y
	)
	{
		unit_z_vector_to_z_vector
		(
			unit_z_x, unit_z_y,
			camera_z(environment),
			camera_local_world_x, camera_local_world_y
		);
	}
}
