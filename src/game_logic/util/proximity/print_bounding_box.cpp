#include "game_logic/util/proximity/print_bounding_box.h"

namespace game_logic::util::proximity
{
	std::ostream& print(std::ostream& os, game_state::proximity::Bounding_Box const& box)
	{
		return os << '[' << box.min << ", " << box.max << ']';
	}
}
