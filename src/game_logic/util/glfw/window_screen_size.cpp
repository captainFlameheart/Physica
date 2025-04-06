#include "game_logic/util/glfw/window_screen_size.h"
#include "game_environment/environment.h"

void game_logic::util::glfw::window_screen_size
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
