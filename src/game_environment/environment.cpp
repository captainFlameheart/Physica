#include "game_environment/environment.h"
#include <GLFW/glfw3.h>

namespace game_environment
{
	Environment& from(GLFWwindow* window)
	{
		return *static_cast<Environment*>(glfwGetWindowUserPointer(window));
	}
}
