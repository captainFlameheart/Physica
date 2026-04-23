#include "game_logic/initialize/query_layouts/include.h"
#include "game_environment/Environment.h"
#include "game_logic/shader_util/query/include.h"
#include <iostream>

namespace game_logic::initialize::query_layouts
{
	void query_on_events_polled_data_layout(game_environment::Environment& environment, GLuint program)
	{
		::game_logic::shader_util::query::query_uniform_variable_offset
		(
			program, "On_Events_Polled_Data.cursor_position_framebuffer_size",
			environment.state.layouts.on_events_polled_data.cursor_position_framebuffer_size_properties
		);
		::game_logic::shader_util::query::query_uniform_buffer_data_size
		(
			program, "On_Events_Polled_Data",
			environment.state.layouts.on_events_polled_data.block_properties
		);
	}
}
