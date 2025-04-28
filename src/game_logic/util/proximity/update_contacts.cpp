#include "game_logic/util/proximity/update_contacts.h"
#include "game_logic/util/proximity/NULL_INDEX.h"
#include "game_logic/util/proximity/separated.h"

namespace game_logic::util::proximity
{
	template <typename Replace_Contact, typename Append_Contact, typename Copy_Contact>
	void update_contacts
	(
		game_state::proximity::Tree& tree, 
		Replace_Contact& replace_contact, 
		Append_Contact& append_contact, 
		Copy_Contact& copy_contact
	)
	{
		// Remove contacts
		for (GLuint i{ 0u }; i < tree.changed_leaf_count; ++i)
		{
			game_state::proximity::Leaf_Change const& leaf_change{ tree.leaf_changes[i] };
			game_state::proximity::Node const& leaf{ tree.nodes[leaf_change.leaf] };
			GLuint contact_index{ leaf.contact };
			while (contact_index != game_logic__util__proximity_NULL_INDEX)
			{
				game_state::proximity::Contact const& contact{ tree.contacts[contact_index] };
				GLuint const leaf_side{ static_cast<GLuint>(leaf_change.leaf == contact.leaf_1) };
				GLuint const other_leaf_side{ (leaf_side + 1u) % 2u };
				GLuint const other_leaf_index{ contact.leafs[other_leaf_side] };
				game_state::proximity::Node const& other_leaf{ tree.nodes[other_leaf_index] };
				game_state::proximity::Neighbor_Pair const& leaf_neighbor_pair{ contact.neighbor_pairs[leaf_side] };
				
				if (separated(leaf.bounding_box, other_leaf.bounding_box))
				{
					// Make previous point to next
					if (leaf_neighbor_pair.previous == game_logic__util__proximity_NULL_INDEX)
					{
						leaf.contact = leaf_neighbor_pair.next;
					}
					else
					{
						game_state::proximity::Contact const& previous_contact{ tree.contacts[leaf_neighbor_pair.previous] };
						GLuint const previous_leaf_side{ static_cast<GLuint>(leaf_change.leaf == previous_contact.leaf_1) };
						previous_contact.neighbor_pairs[previous_leaf_side].next = leaf_neighbor_pair.next;
					}

					// Make next point to previous
					if (leaf_neighbor_pair.next != game_logic__util__proximity_NULL_INDEX)
					{
						game_state::proximity::Contact const& next_contact{ tree.contacts[leaf_neighbor_pair.next] };
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
						game_state::proximity::Contact const& previous_other_contact{ tree.contacts[other_leaf_neighbor_pair.previous] };
						GLuint const previous_other_leaf_side{ static_cast<GLuint>(other_leaf_index == previous_other_contact.leaf_1) };
						previous_other_contact.neighbor_pairs[previous_other_leaf_side].next = other_leaf_neighbor_pair.next;
					}

					// Make other next point to other previous
					if (other_leaf_neighbor_pair.next != game_logic__util__proximity_NULL_INDEX)
					{
						game_state::proximity::Contact const& next_other_contact{ tree.contacts[other_leaf_neighbor_pair.next] };
						GLuint const next_other_leaf_side{ static_cast<GLuint>(other_leaf_index == next_other_contact.leaf_1) };
						next_other_contact.neighbor_pairs[next_other_leaf_side].previous = other_leaf_neighbor_pair.previous;
					}
				};

				contact_index = leaf_neighbor_pair.next;
			}
		}
	}
}
