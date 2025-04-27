#pragma once
#include "glad_glfw.h"
#include "game_state/proximity/Tree.h"
#include "game_environment/environment.h"

namespace game_logic::util::proximity
{
	template <typename FreeContact, typename AddContact>
	inline void resolve_leaf_changes
	(
		game_environment::Environment& environment, 
		FreeContact& free_contact, AddContact& add_contact,
	)
	{

	}
}
