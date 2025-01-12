#pragma once
#include <GLFW/glfw3.h>
#include "game_state.h"

void render(double const processed_time, Game_State const& game_state, GLFWwindow const *window, double lag);
