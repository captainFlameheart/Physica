#pragma once
#include "game_environment/environment.h"

namespace game_logic::on_framebuffer_size_changed
{
	void on_framebuffer_size_changed(game_environment::Environment& environment, int width, int height);
}
