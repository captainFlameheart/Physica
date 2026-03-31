#pragma once

namespace game_state::layouts::bounding_volume_hierarchy
{
	struct height
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
			height height_state;
			GLint height_properties[sizeof(height) / sizeof(GLint)];
		};
		union
		{
			Block block_state;
			GLint block_properties[sizeof(Block) / sizeof(GLint)];
		};
	};
}
