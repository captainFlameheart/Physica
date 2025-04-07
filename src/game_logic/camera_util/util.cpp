#include "game_logic/camera_util/util.h"
#include "game_logic/macros.h"
#include "game_logic/util/spatial/TO_RADIANS.h"

namespace game_logic::camera_util
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

	void increase_camera_xy
	(
		game_environment::Environment& environment,
		GLint const x_increase, GLint const y_increase
	)
	{
		environment.state.camera.xy.x += x_increase;
		environment.state.camera.xy.y += y_increase;
	}

	void increase_camera_angle
	(
		game_environment::Environment& environment,
		GLint const angle_increase
	)
	{
		environment.state.camera.angle += angle_increase;
		// TODO: Make sure to not loose precision due to large angles
		GLfloat const radians{ game_logic__util__spatial_TO_RADIANS(environment, environment.state.camera.angle) };
		GLfloat const right_x{ cos(radians) };
		GLfloat const right_y{ sin(radians) };
		environment.state.camera.view_rotation.column_0[0] = right_x;
		environment.state.camera.view_rotation.column_0[1] = -right_y;
		environment.state.camera.view_rotation.column_1[0] = right_y;
		environment.state.camera.view_rotation.column_1[1] = right_x;
	}
}
