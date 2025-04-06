#include "game_logic/util/glfw/window_screen_cursor_position.h"

void game_logic::util::glfw::window_screen_cursor_position
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
