#include "game_logic/util/proximity/overlap.h"

namespace game_logic::util::proximity
{
	bool overlap
	(
		game_state::proximity::Bounding_Box const& box_0,
		game_state::proximity::Bounding_Box const& box_1
	)
	{
		return (
			box_0.min.x <= box_1.max.x && box_0.min.y <= box_1.max.y && 
			box_0.max.x >= box_1.min.x && box_0.max.y >= box_1.min.y
		);
	}
}
