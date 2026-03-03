#include "game_logic/initialize/query_layouts/include.h"
#include "game_environment/Environment.h"
#include "game_logic/shader_util/query/include.h"
#include <iostream>

namespace game_logic::initialize::query_layouts
{
	void query_key_event_layout(game_environment::Environment& environment, GLuint program)
	{
		::game_logic::shader_util::query::query_uniform_variable_offset
		(
			program, "Key_Event.key_scancode_action_mods",
			environment.state.layouts.key_event.key_scancode_action_mods_properties
		);
		::game_logic::shader_util::query::query_uniform_buffer_data_size
		(
			program, "Key_Event",
			environment.state.layouts.key_event.block_properties
		);
	}
}
