#pragma once
#include "game.h"

struct Full_Game_State
{
	double &tick_delta_time;
	unsigned &max_ticks_per_frame;
	Game_State &game_state;
	double &processed_time;
};

Full_Game_State &get_full_game_state(GLFWwindow* window);
