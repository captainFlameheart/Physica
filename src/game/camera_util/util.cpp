#include "game/camera_util/util.h"

namespace game::camera_util
{
	void camera_local_unit_z_coordinate_to_camera_local_coordinate
	(
		GLfloat const camera_local_unit_z_coordinate, GLfloat const z,
		GLfloat* camera_local_coordinate
	)
	{
		*camera_local_coordinate = camera_local_unit_z_coordinate * z;
	}

	void camera_local_unit_z_coordinate_to_camera_local_world_coordinate
	(
		game_environment::Environment const& environment,
		GLfloat const camera_local_unit_z_coordinate,
		GLfloat* camera_local_world_coordinate
	)
	{
		camera_local_unit_z_coordinate_to_camera_local_coordinate
		(
			camera_local_unit_z_coordinate, environment.state.camera.z,
			camera_local_world_coordinate
		);
	}

	void camera_local_unit_z_x_to_camera_local_x
	(
		GLfloat const camera_local_unit_z_x, GLfloat const z,
		GLfloat* camera_local_x
	)
	{
		camera_local_unit_z_coordinate_to_camera_local_coordinate
		(
			camera_local_unit_z_x, z, 
			camera_local_x
		);
	}

	void camera_local_unit_z_x_to_camera_local_world_x
	(
		game_environment::Environment const& environment,
		GLfloat const camera_local_unit_z_x,
		GLfloat* camera_local_world_x
	)
	{
		camera_local_unit_z_x_to_camera_local_x
		(
			camera_local_unit_z_x, environment.state.camera.z,
			camera_local_world_x
		);
	}

	void camera_local_unit_z_y_to_camera_local_y
	(
		GLfloat const camera_local_unit_z_y, GLfloat const z,
		GLfloat* camera_local_y
	)
	{
		camera_local_unit_z_coordinate_to_camera_local_coordinate
		(
			camera_local_unit_z_y, z,
			camera_local_y
		);
	}

	void camera_local_unit_z_y_to_camera_local_world_y
	(
		game_environment::Environment const& environment,
		GLfloat const camera_local_unit_z_y,
		GLfloat* camera_local_world_y
	)
	{
		camera_local_unit_z_y_to_camera_local_y
		(
			camera_local_unit_z_y, environment.state.camera.z, 
			camera_local_world_y
		);
	}

	void camera_local_unit_z_vector_to_camera_local_vector
	(
		GLfloat const camera_local_unit_z_x, 
		GLfloat const camera_local_unit_z_y, 
		GLfloat const z, 
		GLfloat* camera_local_x, GLfloat* camera_local_y
	)
	{
		camera_local_unit_z_x_to_camera_local_x(
			camera_local_unit_z_x, z,
			camera_local_x
		);
		camera_local_unit_z_y_to_camera_local_y(
			camera_local_unit_z_y, z,
			camera_local_y
		);
	}

	void camera_local_unit_z_vector_to_camera_local_world_vector
	(
		game_environment::Environment const& environment,
		GLfloat const camera_local_unit_z_x,
		GLfloat const camera_local_unit_z_y,
		GLfloat* camera_local_world_x, GLfloat* camera_local_world_y
	)
	{
		camera_local_unit_z_vector_to_camera_local_vector
		(
			camera_local_unit_z_x, camera_local_unit_z_y,
			environment.state.camera.z,
			camera_local_world_x, camera_local_world_y
		);
	}
}
