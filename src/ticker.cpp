#include <glad/glad.h>
#include "ticker.h"
#include <iostream>

void tick(double const ticked_time, Game_State &game_state, double const delta_time)
{
	game_state.x += 1.0 * delta_time;
}
