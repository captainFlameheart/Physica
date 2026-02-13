#include "RUN_LEGACY.h"
#if RUN_LEGACY == 1
#include "game_logic/window_to_world/window_to_world.h"
#include "game_logic/window_to_camera/window_to_camera.h"
#include "game_logic/util/glfw/window_screen_cursor_position.h"
#include "game_logic/util/camera/local_world_vector_to_world_vector.h"
#include "game_logic/util/camera/local_world_position_to_world_position.h"

namespace game_logic::window_to_world
{
	void window_screen_position_to_world_vector
	(
		game_environment::Environment const& environment,
		GLfloat const window_screen_x, GLfloat const window_screen_y,
		GLint* world_x, GLint* world_y
	)
	{
		GLfloat camera_local_world_x, camera_local_world_y;
		window_to_camera::window_screen_position_to_camera_local_world_vector
		(
			environment,
			window_screen_x, window_screen_y,
			&camera_local_world_x, &camera_local_world_y
		);
		util::camera::local_world_vector_to_world_vector
		(
			environment,
			camera_local_world_x, camera_local_world_y,
			world_x, world_y
		);
	}

	void window_screen_cursor_position_to_world_vector
	(
		game_environment::Environment const& environment,
		GLint* world_x, GLint* world_y
	)
	{
		double window_screen_cursor_x, window_screen_cursor_y;
		game_logic::util::glfw::window_screen_cursor_position
		(
			environment, &window_screen_cursor_x, &window_screen_cursor_y
		);
		window_screen_position_to_world_vector(
			environment,
			window_screen_cursor_x, window_screen_cursor_y,
			world_x, world_y
		);
	}

	void window_screen_position_to_world_position
	(
		game_environment::Environment const& environment,
		double const window_screen_x, double const window_screen_y,
		GLint* world_x, GLint* world_y
	)
	{
		GLfloat camera_local_world_x;
		GLfloat camera_local_world_y;
		window_to_camera::window_screen_position_to_camera_local_world_vector
		(
			environment,
			window_screen_x, window_screen_y,
			&camera_local_world_x, &camera_local_world_y
		);
		util::camera::local_world_position_to_world_position
		(
			environment,
			camera_local_world_x, camera_local_world_y,
			world_x, world_y
		);
	}

	void window_screen_cursor_position_to_world_position
	(
		game_environment::Environment const& environment,
		GLint* world_x, GLint* world_y
	)
	{
		double window_screen_cursor_x, window_screen_cursor_y;
		game_logic:util::glfw::window_screen_cursor_position
		(
			environment,
			&window_screen_cursor_x, &window_screen_cursor_y
		);
		window_screen_position_to_world_position
		(
			environment,
			window_screen_cursor_x, window_screen_cursor_y,
			world_x, world_y
		);
	}
}
#endif
