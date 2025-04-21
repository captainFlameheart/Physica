#pragma once
#include "game_state/proximity/Bounding_Box.h"

namespace game_logic::util::proximity
{
	inline void wrap
	(
		game_state::proximity::Bounding_Box const& box_0, 
		game_state::proximity::Bounding_Box const& box_1, 
		game_state::proximity::Bounding_Box& result
	);
}
