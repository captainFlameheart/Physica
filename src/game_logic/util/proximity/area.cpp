#include "game_logic/util/proximity/area.h"

namespace game_logic::util::proximity
{
	GLint area(game_state::proximity::Bounding_Box const& bounding_box)
	{
		return 
			(bounding_box.max.x - bounding_box.min.x) * 
			(bounding_box.max.y - bounding_box.min.y);
	}
}
