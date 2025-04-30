#pragma once
#include "glad_glfw.h"
#include "game_logic/util/proximity/print_leaf_contacts.h"
#include "game_logic/util/proximity/NULL_INDEX.h"

namespace game_logic::util::proximity
{
	void print_leaf_contacts
	(
		std::ostream& os,
		game_state::proximity::Tree const& tree,
		GLuint const leaf_index
	)
	{
		os << leaf_index << ": ";
		game_state::proximity::Node const& leaf{ tree.nodes[leaf_index] };
		GLuint contact_index{ leaf.contact };
		while (contact_index != game_logic__util__proximity_NULL_INDEX)
		{
			game_state::proximity::Contact const& contact{ tree.contacts[contact_index] };
			GLuint side{ static_cast<GLuint>(leaf_index == contact.leaf_1) };
			GLuint other_side{ (side + 1u) % 2u };
			os << '(' << contact_index << ", " << contact.leafs[other_side] << "), ";
			contact_index = contact.neighbor_pairs[side].next;
		}
		os << std::endl;
	}
}
