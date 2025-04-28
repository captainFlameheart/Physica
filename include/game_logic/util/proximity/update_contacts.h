#pragma once
#include "glad_glfw.h"
#include "game_state/proximity/Tree.h"

namespace game_logic::util::proximity
{
	template <typename Replace_Contact, typename Append_Contact, typename Copy_Contact>
	void update_contacts
	(
		game_state::proximity::Tree& tree,
		Replace_Contact& replace_contact,
		Append_Contact& append_contact,
		Copy_Contact& copy_contact
	);
}
