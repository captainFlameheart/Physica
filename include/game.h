#pragma once
#include <GLFW/glfw3.h>
#include "full_game_state.h"

void initialize_game_state(Full_Game_State &game_state);

void on_key_event(
	Full_Game_State &game_state,
	int const key,
	int const scancode,
	int const action,
	int const mods
);

void on_cursor_event(
	Full_Game_State &game_state,
	double const x_pos,
	double const y_pos
);

void on_mouse_button_event(
	Full_Game_State &game_state,
	int const button,
	int const action,
	int const mods
);

void on_scroll_event(
	Full_Game_State &game_state,
	double const x_offset,
	double const y_offset
);

void tick(Full_Game_State &game_state);

void render(Full_Game_State &game_state);

void free_game_state(Full_Game_State &game_state);
