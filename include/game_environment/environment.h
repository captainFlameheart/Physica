#pragma once
//#include <GLFW/glfw3.h>
#include "glad_glfw.h"
#include "game_state/state.h"

namespace game_environment
{
	struct Environment
	{
		GLFWwindow* window;
		GLint lag;
		GLuint ticks_this_frame;
		game_state::State state;
	};

	Environment& from(GLFWwindow* window);
}
