#pragma once
#include <GLFW/glfw3.h>
#include "game_state.h"

void tick(double const processed_time, Game_State &game_state, double const delta_time);
