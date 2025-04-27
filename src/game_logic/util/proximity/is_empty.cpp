#include "game_logic/util/proximity/is_empty.h"
#include "game_logic/util/proximity/NULL_INDEX.h"

namespace game_logic::util::proximity
{
	bool is_empty(game_state::proximity::Tree const& tree)
	{
		return tree.root == game_logic__util__proximity_NULL_INDEX;
	}
}
