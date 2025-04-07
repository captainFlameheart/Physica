#pragma once
#include "game_logic/window_to_camera/window_to_camera.h"
#include "game_logic/macros.h"
#include "game_logic/util/glfw/window_screen_size.h"
#include "game_logic/util/glfw/window_screen_cursor_position.h"
#include "game_logic/util/projection/INVERSE_SCALE_X.h"
#include "game_logic/util/projection/INVERSE_SCALE_Y.h"
#include "game_logic/util/camera/normalized_x_to_unit_z_x.h"
#include "game_logic/util/camera/normalized_y_to_unit_z_y.h"
#include "game_logic/util/camera/unit_z_vector_to_camera_local_world_vector.h"

namespace game_logic::window_to_camera
{
	void window_screen_x_to_camera_local_unit_z_x
	(
		game_environment::Environment const& environment,
		int const window_screen_width, double const window_screen_x,
		GLfloat* camera_local_unit_x
	)
	{
		util::camera::normalized_x_to_unit_z_x
		(
			environment, 
			static_cast<GLfloat>((window_screen_x / window_screen_width) * 2.0 - 1.0), 
			camera_local_unit_x
		);
	}

	void window_screen_y_to_camera_local_unit_z_y
	(
		game_environment::Environment const& environment,
		int const window_screen_height, double const window_screen_y,
		GLfloat* camera_local_unit_y
	)
	{
		util::camera::normalized_y_to_unit_z_y
		(
			environment,
			static_cast<GLfloat>(1.0 - (window_screen_y / window_screen_height) * 2.0),
			camera_local_unit_y
		);
	}

	void window_screen_position_to_camera_local_unit_z_vector
	(
		game_environment::Environment const& environment,
		double const window_screen_x, double const window_screen_y,
		GLfloat* camera_local_unit_z_x, GLfloat* camera_local_unit_z_y
	)
	{
		int window_screen_width;
		int window_screen_height;
		game_logic::util::glfw::window_screen_size(environment, &window_screen_width, &window_screen_height);

		window_screen_x_to_camera_local_unit_z_x
		(
			environment, window_screen_width, window_screen_x, camera_local_unit_z_x
		);
		window_screen_y_to_camera_local_unit_z_y
		(
			environment, window_screen_height, window_screen_y, camera_local_unit_z_y
		);
	}

	void window_screen_cursor_position_to_camera_local_unit_z_vector
	(
		game_environment::Environment const& environment,
		GLfloat* camera_local_unit_z_x, GLfloat* camera_local_unit_z_y
	)
	{
		double window_screen_cursor_x, window_screen_cursor_y;
		game_logic::util::glfw::window_screen_cursor_position
		(
			environment, &window_screen_cursor_x, &window_screen_cursor_y
		);
		window_screen_position_to_camera_local_unit_z_vector
		(
			environment,
			window_screen_cursor_x, window_screen_cursor_y,
			camera_local_unit_z_x, camera_local_unit_z_y
		);
	}

	void window_screen_position_to_camera_local_world_vector
	(
		game_environment::Environment const& environment,
		double const window_screen_x, double const window_screen_y,
		GLfloat* camera_local_world_x, GLfloat* camera_local_world_y
	)
	{
		GLfloat camera_local_unit_z_x;
		GLfloat camera_local_unit_z_y;
		window_screen_position_to_camera_local_unit_z_vector
		(
			environment,
			window_screen_x, window_screen_y,
			&camera_local_unit_z_x, &camera_local_unit_z_y
		);

		game_logic::util::camera::unit_z_vector_to_camera_local_world_vector
		(
			environment, 
			camera_local_unit_z_x, camera_local_unit_z_y, 
			camera_local_world_x, camera_local_world_y
		);
	}

	void window_screen_cursor_position_to_camera_local_world_vector
	(
		game_environment::Environment const& environment,
		GLfloat* camera_local_world_x, GLfloat* camera_local_world_y
	)
	{
		double window_screen_cursor_x, window_screen_cursor_y;
		game_logic::util::glfw::window_screen_cursor_position
		(
			environment, &window_screen_cursor_x, &window_screen_cursor_y
		);
		window_screen_position_to_camera_local_world_vector
		(
			environment,
			window_screen_cursor_x, window_screen_cursor_y,
			camera_local_world_x, camera_local_world_y
		);
	}
}
