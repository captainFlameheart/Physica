#pragma once
#include "glad_glfw.h"
#include "game_state/proximity/Tree.h"

namespace game_logic::util::proximity
{
	bool is_empty(game_state::proximity::Tree const& tree);
}
