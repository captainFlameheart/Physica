#pragma once
#include <GLFW/glfw3.h>

struct Game_State
{
	double x;
	GLuint shader;
	GLuint x_uniform;
	GLuint vbo;
	GLuint vao;
};

void initialize_game_state(
	GLFWwindow *window,
	double &tick_delta_time, 
	unsigned &max_ticks_per_frame, 
	Game_State &game_state
);

void on_key_event(
	GLFWwindow *window,
	double const tick_delta_time,
	unsigned const max_ticks_per_frame,
	Game_State &game_state, 
	double const processed_time,
	int const key, 
	int const scancode,
	int const action,
	int const mods
);

void on_cursor_event(
	GLFWwindow *window,
	double const tick_delta_time,
	unsigned const max_ticks_per_frame,
	Game_State &game_state,
	double const processed_time,
	double const x_pos,
	double const y_pos
);

void on_mouse_button_event(
	GLFWwindow *window,
	double const tick_delta_time,
	unsigned const max_ticks_per_frame,
	Game_State &game_state,
	double const processed_time,
	int const button,
	int const action,
	int const mods
);

void on_scroll_event(
	GLFWwindow *window,
	double const tick_delta_time,
	unsigned const max_ticks_per_frame,
	Game_State &game_state,
	double const processed_time,
	double const x_offset,
	double const y_offset
);

void tick(
	GLFWwindow *window,
	double const delta_time,
	unsigned const max_ticks_per_frame,
	Game_State &game_state, 
	double const processed_time,
	unsigned const ticks_this_frame
);

void render(
	GLFWwindow const* window,
	double const tick_delta_time, 
	unsigned const max_ticks_per_frame,
	Game_State const& game_state,
	double const processed_time, 
	double lag
);

void free_game_state(
	GLFWwindow *window,
	double const tick_delta_time,
	unsigned const max_ticks_per_frame,
	Game_State &game_state,
	double const processed_time
);
