#pragma once
#include "game_logic/cursor_types/initialize.h"

namespace game_logic::cursor_types
{
	void initialize(game_environment::Environment& environment)
	{
		game_state::cursor_types::Cursor_Type_Set& cursor_types
		{ 
			environment.state.cursor_types
		};
		cursor_types.grab_cursor = glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);
		cursor_types.point_cursor = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
		cursor_types.move_cursor = glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);
	}
}
