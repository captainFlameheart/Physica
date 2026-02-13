#pragma once
//#include <GLFW/glfw3.h>
#include "glad_glfw.h"
#include "RUN_LEGACY.h"
#if RUN_LEGACY
#include "legacy/game_state/game.h"
#else
#include "game_state/game.h"
#endif


namespace game_environment
{
	struct Environment
	{
		GLFWwindow* window;
		GLfloat lag;//GLint lag;
		GLuint ticks_this_frame;
#if RUN_LEGACY == 1
		legacy::game_state::Game state;
#else
		::game_state::Game state;
#endif
	};

	Environment& from(GLFWwindow* window);
}
