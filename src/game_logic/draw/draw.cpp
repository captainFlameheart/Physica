#include "game_environment/environment.h"
#include <iostream>

namespace game_logic::draw
{
	void draw(game_environment::Environment& environment)
	{
		glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
	}
}
