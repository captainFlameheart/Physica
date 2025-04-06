#pragma once
#include "game_environment/environment.h"

namespace game_logic::camera_to_world
{
	void camera_local_world_vector_to_world_vector
	(
		game_environment::Environment const& environment,
		GLfloat const camera_local_world_x, GLfloat const camera_local_world_y,
		GLint* world_vector_x, GLint* world_vector_y
	);

	void camera_world_offset_to_world_position
	(
		game_environment::Environment const& environment,
		GLint const camera_world_offset_x, GLint const camera_world_offset_y,
		GLint* world_position_x, GLint* world_position_y
	);

	void camera_local_world_position_to_world_position(
		game_environment::Environment const& environment,
		GLfloat const camera_local_world_x, GLfloat const camera_local_world_y,
		GLint* world_x, GLint* world_y
	);
}
