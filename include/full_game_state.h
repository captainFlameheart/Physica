#pragma once
#include <GLFW/glfw3.h>
#include "game/game_state.h"

struct Full_Game_State
{
	GLFWwindow *window;
	//GLint real_time;
	//GLint processed_time;
	GLint lag;
	GLuint ticks_this_frame;
	Game_State state;
};

Full_Game_State &get_full_game_state(GLFWwindow *window);
