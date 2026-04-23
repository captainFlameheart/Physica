#pragma once

namespace game_state::layouts::on_events_polled_data
{
	struct cursor_position_framebuffer_size
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
			cursor_position_framebuffer_size cursor_position_framebuffer_size_state;
			GLint cursor_position_framebuffer_size_properties[sizeof(cursor_position_framebuffer_size) / sizeof(GLint)];
		};
		union
		{
			Block block_state;
			GLint block_properties[sizeof(Block) / sizeof(GLint)];
		};
	};
}
