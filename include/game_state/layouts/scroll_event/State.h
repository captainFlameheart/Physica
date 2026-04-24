#pragma once

namespace game_state::layouts::scroll_event
{
	struct offsets
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
			offsets offsets_state;
			GLint offsets_properties[sizeof(offsets) / sizeof(GLint)];
		};
		union
		{
			Block block_state;
			GLint block_properties[sizeof(Block) / sizeof(GLint)];
		};
	};
}
