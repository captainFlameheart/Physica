#pragma once
#include <glad/glad.h>

struct Game_State
{
	double x;
	GLuint shader;
	GLuint x_uniform;
	GLuint vbo;
	GLuint vao;
};

void initialize_game_state(Game_State &game_state);

void free_game_state(Game_State& game_state);
