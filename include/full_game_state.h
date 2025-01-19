#pragma once
#include <GLFW/glfw3.h>
#include "game_state.h"

struct Full_Game_State
{
	GLFWwindow *window;
	double real_time;
	double processed_time;
	double lag;
	double tick_delta_time;
	unsigned ticks;
	unsigned max_ticks_per_frame;
	Game_State state;
};

Full_Game_State &get_full_game_state(GLFWwindow *window);
