#include "glad_glfw.h"
#include "game_environment/Environment.h"
#include "game_logic/initialize/glfw/include.h"

namespace game_logic::initialize::glfw
{
	void initialize(game_environment::Environment& environment)
	{
		glfwSetInputMode(environment.window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

		glfwGetFramebufferSize(environment.window, &environment.state.glfw.framebuffer_width, &environment.state.glfw.framebuffer_height);
		glViewport(0, 0, environment.state.glfw.framebuffer_width, environment.state.glfw.framebuffer_height);
	}
}
