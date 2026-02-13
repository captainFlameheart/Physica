#pragma once
#include "game_logic/cursor_types/free.h"
#include "game_state/cursor_types/cursor_type_set.h"
#include "RUN_LEGACY.h"

namespace game_logic::cursor_types
{
	void free(game_environment::Environment& environment)
	{
#if RUN_LEGACY == 1
		::game_state::cursor_types::Cursor_Type_Set& cursor_types
		{
			environment.state.cursor_types
		};
		glfwDestroyCursor(cursor_types.grab_cursor);
		glfwDestroyCursor(cursor_types.point_cursor);
		glfwDestroyCursor(cursor_types.move_cursor);
#endif
	}
}
