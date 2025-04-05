#include "game/glfw/glfw.h"

namespace game::glfw
{
	void get_window_screen_size
	(
		game_environment::Environment const& environment,
		int* window_screen_width, int* window_screen_height
	)
	{
		glfwGetWindowSize(
			environment.window, 
			window_screen_width, window_screen_height
		);
	}

	void get_window_screen_cursor_position
	(
		game_environment::Environment const& environment,
		double* window_screen_cursor_x, double* window_screen_cursor_y
	)
	{
		glfwGetCursorPos(
			environment.window, 
			window_screen_cursor_x, window_screen_cursor_y
		);
	}
}
