#pragma once
#include "glad_glfw.h"
#include "game_state/proximity/Tree.h"
#include "game_logic/util/proximity/NULL_INDEX.h"
#include "game_logic/util/proximity/overlap.h"
#include "game_logic/util/proximity/separated.h"
#include "game_logic/util/proximity/print_leaf_contacts.h"

namespace game_logic::util::proximity
{
	template
	<
		typename On_Removing_Contacts_For,
		typename Remove_Contact
	>
	inline void remove_contacts
	(
		game_state::proximity::Tree& tree, GLuint const max_leaf_count,
		GLuint start_leaf_change_index, GLuint end_leaf_change_index,
		On_Removing_Contacts_For& on_removing_contacts_for,
		Remove_Contact& remove_contact
	)
	{
		for (GLuint i{ start_leaf_change_index }; i < end_leaf_change_index; ++i)
		{
			game_state::proximity::Leaf_Change const& leaf_change{ tree.leaf_changes[i] };
			game_state::proximity::Node& leaf{ tree.nodes[leaf_change.leaf] };
			on_removing_contacts_for(leaf_change.leaf);

			GLuint contact_index{ leaf.contact };
			while (contact_index != game_logic__util__proximity_NULL_INDEX)
			{
				game_state::proximity::Contact const& contact{ tree.contacts[contact_index] };
				GLuint const leaf_side{ static_cast<GLuint>(leaf_change.leaf == contact.leaf_1) };
				GLuint const other_leaf_side{ (leaf_side + 1u) % 2u };
				GLuint const other_leaf_index{ contact.leafs[other_leaf_side] };
				game_state::proximity::Node& other_leaf{ tree.nodes[other_leaf_index] };
				game_state::proximity::Neighbor_Pair const& leaf_neighbor_pair{ contact.neighbor_pairs[leaf_side] };
				GLuint const current_contact_index{ contact_index };
				contact_index = leaf_neighbor_pair.next;

				if (separated(leaf.bounding_box, other_leaf.bounding_box))
				{
					// Make previous point to next
					if (leaf_neighbor_pair.previous == game_logic__util__proximity_NULL_INDEX)
					{
						leaf.contact = leaf_neighbor_pair.next;
					}
					else
					{
						game_state::proximity::Contact& previous_contact{ tree.contacts[leaf_neighbor_pair.previous] };
						GLuint const previous_leaf_side{ static_cast<GLuint>(leaf_change.leaf == previous_contact.leaf_1) };
						previous_contact.neighbor_pairs[previous_leaf_side].next = leaf_neighbor_pair.next;
					}

					// Make next point to previous
					if (leaf_neighbor_pair.next != game_logic__util__proximity_NULL_INDEX)
					{
						game_state::proximity::Contact& next_contact{ tree.contacts[leaf_neighbor_pair.next] };
						GLuint const next_leaf_side{ static_cast<GLuint>(leaf_change.leaf == next_contact.leaf_1) };
						next_contact.neighbor_pairs[next_leaf_side].previous = leaf_neighbor_pair.previous;
					}

					// Make other previous point to other next
					game_state::proximity::Neighbor_Pair const& other_leaf_neighbor_pair{ contact.neighbor_pairs[other_leaf_side] };
					if (other_leaf_neighbor_pair.previous == game_logic__util__proximity_NULL_INDEX)
					{
						other_leaf.contact = other_leaf_neighbor_pair.next;
					}
					else
					{
						game_state::proximity::Contact& previous_other_contact{ tree.contacts[other_leaf_neighbor_pair.previous] };
						GLuint const previous_other_leaf_side{ static_cast<GLuint>(other_leaf_index == previous_other_contact.leaf_1) };
						previous_other_contact.neighbor_pairs[previous_other_leaf_side].next = other_leaf_neighbor_pair.next;
					}

					// Make other next point to other previous
					if (other_leaf_neighbor_pair.next != game_logic__util__proximity_NULL_INDEX)
					{
						game_state::proximity::Contact& next_other_contact{ tree.contacts[other_leaf_neighbor_pair.next] };
						GLuint const next_other_leaf_side{ static_cast<GLuint>(other_leaf_index == next_other_contact.leaf_1) };
						next_other_contact.neighbor_pairs[next_other_leaf_side].previous = other_leaf_neighbor_pair.previous;
					}

					remove_contact(current_contact_index, contact_index);
				};
			}
		}
	}
}
