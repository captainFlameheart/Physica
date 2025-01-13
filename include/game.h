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

void initialize_game_state(double &tick_delta_time, unsigned &max_ticks_per_frame, Game_State &game_state);

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

void free_game_state(Game_State &game_state);
