#pragma once

namespace game_state::layouts::merge_fluence_data
{
	struct Block
	{
		GLint buffer_data_size;
	};

	struct State
	{
		union
		{
			Block block_state;
			GLint block_properties[sizeof(Block) / sizeof(GLint)];
		};
	};
}
