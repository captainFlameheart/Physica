#include "game_logic/initialize/query_layouts/include.h"
#include "game_environment/Environment.h"
#include "game_logic/shader_util/query/include.h"
#include <iostream>

namespace game_logic::initialize::query_layouts
{
	void query_mouse_button_event_layout(game_environment::Environment& environment, GLuint program)
	{
		::game_logic::shader_util::query::query_uniform_variable_offset
		(
			program, "Mouse_Button_Event.button_action_mods",
			environment.state.layouts.mouse_button_event.button_action_mods_properties
		);
		::game_logic::shader_util::query::query_uniform_buffer_data_size
		(
			program, "Mouse_Button_Event",
			environment.state.layouts.mouse_button_event.block_properties
		);
	}
}
