#pragma once
#include "glad_glfw.h"
#include "game_state/proximity/Tree.h"
#include "game_logic/util/proximity/NULL_INDEX.h"
#include "game_logic/util/proximity/overlap.h"
#include "game_logic/util/proximity/separated.h"
#include "game_logic/util/proximity/print_leaf_contacts.h"
/*
namespace game_logic::util::proximity
{
	template <typename Contact_Can_Be_Added, typename Add_Contact>
	inline void insert_contact(
		game_state::proximity::Tree& tree,
		game_state::proximity::Leaf_Change const& leaf_change,
		game_state::proximity::Node& leaf,
		GLuint const other_leaf_index, game_state::proximity::Node& other_leaf,
		Contact_Can_Be_Added& contact_can_be_added, Add_Contact& add_contact
	)
	{
		if (contact_can_be_added(leaf_change.leaf, other_leaf_index))
		{
			GLuint const contact_index{ add_contact(leaf_change.leaf, other_leaf_index) };
			game_state::proximity::Contact& contact{ tree.contacts[contact_index] };
			contact.leaf_0 = leaf_change.leaf;
			contact.leaf_1 = other_leaf_index;

			contact.child_0_neighbor_pair.previous = game_logic__util__proximity_NULL_INDEX;
			contact.child_0_neighbor_pair.next = leaf.contact;
			if (leaf.contact != game_logic__util__proximity_NULL_INDEX)
			{
				game_state::proximity::Contact& next_contact{ tree.contacts[leaf.contact] };
				GLuint const next_side{ static_cast<GLuint>(leaf_change.leaf == next_contact.leaf_1) };
				next_contact.neighbor_pairs[next_side].previous = contact_index;
			}
			leaf.contact = contact_index;

			contact.child_1_neighbor_pair.previous = game_logic__util__proximity_NULL_INDEX;
			contact.child_1_neighbor_pair.next = other_leaf.contact;
			if (other_leaf.contact != game_logic__util__proximity_NULL_INDEX)
			{
				game_state::proximity::Contact& other_next_contact{ tree.contacts[other_leaf.contact] };
				GLuint const other_next_side{ static_cast<GLuint>(other_leaf_index == other_next_contact.leaf_1) };
				other_next_contact.neighbor_pairs[other_next_side].previous = contact_index;
			}
			other_leaf.contact = contact_index;
		}
	}

	template
		<
		typename On_Removing_Contacts_For,
		typename Remove_Contact,
		typename On_Contacts_Removed,
		typename On_Adding_Contacts_For, 
		typename Contact_Can_Be_Added, 
		typename Add_Contact,
		typename On_Contacts_Added
		>
		inline void update_contacts
		(
			game_state::proximity::Tree& tree, GLuint const max_leaf_count,
			On_Removing_Contacts_For& on_removing_contacts_for,
			Remove_Contact& remove_contact,
			On_Contacts_Removed& on_contacts_removed,
			On_Adding_Contacts_For& on_adding_contacts_for,
			Contact_Can_Be_Added& contact_can_be_added, 
			Add_Contact& add_contact,
			On_Contacts_Added& on_contacts_added
		)
	{
		// Remove contacts
		for (GLuint i{ 0u }; i < tree.changed_leaf_count; ++i)
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
		on_contacts_removed();

		// Add contacts
		for (GLuint i{ 0u }; i < tree.changed_leaf_count; ++i)
		{
			game_state::proximity::Leaf_Change const& leaf_change{ tree.leaf_changes[i] };
			game_state::proximity::Node& leaf{ tree.nodes[leaf_change.leaf] };
			on_adding_contacts_for(leaf_change.leaf);

			// The leaf is guaranteed to overlap with the root, so we don't check for that
			game_state::proximity::Node const& root{ tree.nodes[tree.root] };
			tree.nodes_to_visit[0] = root.child_0;
			tree.nodes_to_visit[1] = root.child_1;
			GLuint next_to_visit{ 1u };
			// TODO: See if the while loop can be unrolled somehow 
			// to reduce the amount of condition checks (maybe using gotos???).
			do
			{
				GLuint const node_index{ tree.nodes_to_visit[next_to_visit--] };
				game_state::proximity::Node const& node{ tree.nodes[node_index] };
				if (overlap(leaf.bounding_box, node.bounding_box))
				{
					if (node_index < max_leaf_count)
					{
						game_state::proximity::Node& other_leaf{ tree.nodes[node_index] };
						game_state::proximity::Leaf_Change const& other_leaf_change{ tree.leaf_changes[other_leaf.change] };

						if
						( // Other leaf unchanged
							other_leaf.change >= tree.changed_leaf_count ||
							other_leaf_change.leaf != node_index
						)
						{
							if (separated(leaf_change.from, other_leaf.bounding_box))
							{
								insert_contact(tree, leaf_change, leaf, node_index, other_leaf, contact_can_be_added, add_contact);
							}
						}
						else if
						(
							leaf_change.leaf < node_index &&
							separated(leaf_change.from, other_leaf_change.from)
						)
						{
							insert_contact(tree, leaf_change, leaf, node_index, other_leaf, contact_can_be_added, add_contact);
						}
					}
					else
					{
						tree.nodes_to_visit[++next_to_visit] = node.child_0;
						tree.nodes_to_visit[++next_to_visit] = node.child_1;
					}
				}
			} while (next_to_visit != game_logic__util__proximity_NULL_INDEX);
		}
		on_contacts_added();
	}
}
*/