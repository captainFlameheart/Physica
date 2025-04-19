#pragma once
#include "Contact.h"

namespace game_logic::util::proximity
{
	struct Tree
	{
		Leaf_Change* leaf_changes;
		Inner_Node* inner_nodes;
		Leaf* leafs;
		Contact* contacts;
	};
}