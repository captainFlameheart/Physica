#pragma once

namespace game_state::layouts::mouse_button_event
{
	struct button_action_mods
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
			button_action_mods button_action_mods_state;
			GLint button_action_mods_properties[sizeof(button_action_mods) / sizeof(GLint)];
		};
		union
		{
			Block block_state;
			GLint block_properties[sizeof(Block) / sizeof(GLint)];
		};
	};
}
