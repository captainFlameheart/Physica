#include "game_environment/environment.h"
#include <GLFW/glfw3.h>

game_environment::Environment& game_environment::from(GLFWwindow* window)
{
	return *static_cast<game_environment::Environment*>(glfwGetWindowUserPointer(window));
}
