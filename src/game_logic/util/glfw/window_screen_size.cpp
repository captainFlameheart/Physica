#include "game_logic/util/glfw/window_screen_size.h"
#include "game_logic/util/glfw/window.h"

namespace game_logic::util::glfw
{
	void window_screen_size
	(
		game_environment::Environment const& environment,
		int* window_screen_width, int* window_screen_height
	)
	{
		glfwGetWindowSize(
			window(environment),
			window_screen_width, window_screen_height
		);
	}
}
