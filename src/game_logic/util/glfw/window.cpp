#include "game_logic/util/glfw/window.h"

namespace game_logic::util::glfw
{
	GLFWwindow* window(game_environment::Environment const& environment)
	{
		return environment.window;
	}
}

