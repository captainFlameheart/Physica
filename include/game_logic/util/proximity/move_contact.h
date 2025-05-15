#pragma once
#include "glad_glfw.h"
#include "game_state/proximity/Tree.h"
#include "game_logic/util/proximity/NULL_INDEX.h"
#include "game_logic/util/proximity/overlap.h"
#include "game_logic/util/proximity/separated.h"
#include "game_logic/util/proximity/print_leaf_contacts.h"

namespace game_logic::util::proximity
{
	inline void move_contact(
		game_state::proximity::Tree& tree,
		GLuint const from, GLuint const to,
		GLuint& next_contact_index
	)
	{
		game_state::proximity::Contact const& moved_contact{ tree.contacts[from] };
		tree.contacts[to] = moved_contact;

		if (moved_contact.child_0_neighbor_pair.previous == game_logic__util__proximity_NULL_INDEX)
		{
			tree.nodes[moved_contact.leaf_0].contact = to;
		}
		else
		{
			game_state::proximity::Contact& previous{ tree.contacts[moved_contact.child_0_neighbor_pair.previous] };
			GLuint const side{ static_cast<GLuint>(previous.child_1_neighbor_pair.next == from) };
			previous.neighbor_pairs[side].next = to;
		}
		if (moved_contact.child_0_neighbor_pair.next != game_logic__util__proximity_NULL_INDEX)
		{
			game_state::proximity::Contact& next{ tree.contacts[moved_contact.child_0_neighbor_pair.next] };
			GLuint const side{ static_cast<GLuint>(next.child_1_neighbor_pair.previous == from) };
			next.neighbor_pairs[side].previous = to;
		}

		if (moved_contact.child_1_neighbor_pair.previous == game_logic__util__proximity_NULL_INDEX)
		{
			tree.nodes[moved_contact.leaf_1].contact = to;
		}
		else
		{
			game_state::proximity::Contact& previous{ tree.contacts[moved_contact.child_1_neighbor_pair.previous] };
			GLuint const side{ static_cast<GLuint>(previous.child_1_neighbor_pair.next == from) };
			previous.neighbor_pairs[side].next = to;
		}
		if (moved_contact.child_1_neighbor_pair.next != game_logic__util__proximity_NULL_INDEX)
		{
			game_state::proximity::Contact& next{ tree.contacts[moved_contact.child_1_neighbor_pair.next] };
			GLuint const side{ static_cast<GLuint>(next.child_1_neighbor_pair.previous == from) };
			next.neighbor_pairs[side].previous = to;
		}

		if (next_contact_index == from)
		{
			next_contact_index = to;
		}
	}
}
