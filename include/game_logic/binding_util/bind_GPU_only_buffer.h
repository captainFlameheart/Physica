#pragma once
#include "game_environment/environment.h"

namespace game_logic::binding_util
{
	void bind_GPU_only_buffer(game_environment::Environment& environment, GLuint index);
}