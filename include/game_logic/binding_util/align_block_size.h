#pragma once
#include "game_environment/environment.h"

namespace game_logic::binding_util
{
	GLuint align_block_size(game_environment::Environment& environment, GLuint block_size, GLuint alignment);
}
