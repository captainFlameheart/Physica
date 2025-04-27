#include "game_logic/util/proximity/wrap.h"

namespace game_logic::util::proximity
{
	void wrap
	(
		game_state::proximity::Bounding_Box const& box_0,
		game_state::proximity::Bounding_Box const& box_1,
		game_state::proximity::Bounding_Box& result
	)
	{
		result.min.x = std::min(box_0.min.x, box_1.min.x);
		result.min.y = std::min(box_0.min.y, box_1.min.y);
		result.max.x = std::max(box_0.max.x, box_1.max.x);
		result.max.y = std::max(box_0.max.y, box_1.max.y);
	}
}
