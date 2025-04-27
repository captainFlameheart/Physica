#include "game_logic/util/proximity/initialize.h"
#include "game_logic/util/proximity/game_logic__util__proximity_MAX_LEAF_COUNT.h"

namespace game_logic::util::proximity
{
	void initialize(game_state::proximity::Tree& tree)
	{
		tree.leaf_changes = new game_state::proximity::Leaf_Change[game_logic__util__proximity_MAX_LEAF_COUNT];
		tree.nodes = new game_state::proximity::Node[2u * game_logic__util__proximity_MAX_LEAF_COUNT - 1u];
	}
}
