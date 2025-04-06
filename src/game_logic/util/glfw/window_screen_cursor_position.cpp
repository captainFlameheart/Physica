#include "game_logic/util/glfw/window_screen_cursor_position.h"
#include "game_logic/util/glfw/window.h"

namespace game_logic::util::glfw
{
	void window_screen_cursor_position
	(
		game_environment::Environment const& environment,
		double* window_screen_cursor_x, double* window_screen_cursor_y
	)
	{
		glfwGetCursorPos(
			window(environment),
			window_screen_cursor_x, window_screen_cursor_y
		);
	}
}
