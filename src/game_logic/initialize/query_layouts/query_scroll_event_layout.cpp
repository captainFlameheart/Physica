#include "game_logic/initialize/query_layouts/include.h"
#include "game_environment/Environment.h"
#include "game_logic/shader_util/query/include.h"
#include <iostream>

namespace game_logic::initialize::query_layouts
{
	void query_scroll_event_layout(game_environment::Environment& environment, GLuint program)
	{
		::game_logic::shader_util::query::query_uniform_variable_offset
		(
			program, "Scroll_Event.offsets",
			environment.state.layouts.scroll_event.offsets_properties
		);
		::game_logic::shader_util::query::query_uniform_buffer_data_size
		(
			program, "Scroll_Event",
			environment.state.layouts.scroll_event.block_properties
		);
	}
}
