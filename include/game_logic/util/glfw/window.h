#pragma once
#include "game_environment/environment.h"

namespace game_logic::util::glfw
{
	GLFWwindow* window(game_environment::Environment const& environment);
}
