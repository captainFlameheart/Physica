#pragma once

namespace game_state::layouts::key_event
{
	struct key_scancode_action_mods
	{
		GLint offset;
	};

	struct Block
	{
		GLint buffer_data_size;
	};

	struct State
	{
		union
		{
			key_scancode_action_mods key_scancode_action_mods_state;
			GLint key_scancode_action_mods_properties[sizeof(key_scancode_action_mods) / sizeof(GLint)];
		};
		union
		{
			Block block_state;
			GLint block_properties[sizeof(Block) / sizeof(GLint)];
		};
	};
}
