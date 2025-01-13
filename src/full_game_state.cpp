#include "full_game_state.h"
#include <GLFW/glfw3.h>

Full_Game_State& get_full_game_state(GLFWwindow* window)
{
	return *static_cast<Full_Game_State*>(glfwGetWindowUserPointer(window));
}
