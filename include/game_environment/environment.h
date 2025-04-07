#pragma once
//#include <GLFW/glfw3.h>
#include "glad_glfw.h"
#include "game_state/game.h"

namespace game_environment
{
	struct Environment
	{
		GLFWwindow* window;
		GLfloat lag;//GLint lag;
		GLuint ticks_this_frame;
		game_state::Game state;
	};

	Environment& from(GLFWwindow* window);
}
