#include "game_logic/camera_to_world/camera_to_world.h"
#include "game_environment/environment.h"

namespace game_logic::camera_to_world
{
	void camera_local_world_vector_to_world_vector
	(
		game_environment::Environment const& environment,
		GLfloat const camera_local_world_x, GLfloat const camera_local_world_y,
		GLint* world_vector_x, GLint* world_vector_y
	)
	{
		*world_vector_x = static_cast<GLint>
		(
			camera_local_world_x * environment.state.camera.view_rotation.column_0[0] +
			camera_local_world_y * environment.state.camera.view_rotation.column_0[1]
		);
		*world_vector_y = static_cast<GLint>
		(
			camera_local_world_x * environment.state.camera.view_rotation.column_1[0] +
			camera_local_world_y * environment.state.camera.view_rotation.column_1[1]
		);
	}

	void camera_world_offset_to_world_position
	(
		game_environment::Environment const& environment,
		GLint const camera_world_offset_x, GLint const camera_world_offset_y,
		GLint* world_position_x, GLint* world_position_y
	)
	{
		// TODO: Implement and use Vector_2D add function for this
		*world_position_x = environment.state.camera.xy.x + camera_world_offset_x;
		*world_position_y = environment.state.camera.xy.y + camera_world_offset_y;
	}

	void camera_local_world_position_to_world_position(
		game_environment::Environment const& environment,
		GLfloat const camera_local_world_x, GLfloat const camera_local_world_y,
		GLint* world_x, GLint* world_y
	)
	{
		camera_local_world_vector_to_world_vector
		(
			environment, camera_local_world_x, camera_local_world_y, world_x, world_y
		);
		camera_world_offset_to_world_position
		(
			environment, *world_x, *world_y, world_x, world_y
		);
	}
}
