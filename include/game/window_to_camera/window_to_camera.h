#pragma once
#include "game_environment/environment.h"

namespace game::window_to_camera
{
	void window_screen_x_to_camera_local_unit_z_x
	(
		game_environment::Environment const& environment,
		int const window_screen_width, double const window_screen_x,
		GLfloat* camera_local_unit_x
	);

	void window_screen_y_to_camera_local_unit_z_y
	(
		game_environment::Environment const& environment,
		int const window_screen_height, double const window_screen_y,
		GLfloat* camera_local_unit_y
	);

	void window_screen_position_to_camera_local_unit_z_vector
	(
		game_environment::Environment const& environment,
		double const window_screen_x, double const window_screen_y,
		GLfloat* camera_local_unit_z_x, GLfloat* camera_local_unit_z_y
	);

	void window_screen_cursor_position_to_camera_local_unit_z_vector
	(
		game_environment::Environment const& environment,
		GLfloat* camera_local_unit_z_x, GLfloat* camera_local_unit_z_y
	);

	void window_screen_position_to_camera_local_world_vector
	(
		game_environment::Environment const& environment,
		double const window_screen_x, double const window_screen_y,
		GLfloat* camera_local_world_x, GLfloat* camera_local_world_y
	);

	void window_screen_cursor_position_to_camera_local_world_vector
	(
		game_environment::Environment const& environment,
		GLfloat* camera_local_world_x, GLfloat* camera_local_world_y
	);
}
