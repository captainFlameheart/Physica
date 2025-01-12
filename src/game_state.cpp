#include "game_state.h"
#include "util/shader.h"

void initialize_game_state(Game_State &game_state)
{
	game_state.x = 0.0;

	game_state.shader = util::shader::link_files("vertex.vert", "fragment.frag");
	game_state.x_uniform = glGetUniformLocation(game_state.shader, "x");

	glGenVertexArrays(1, &game_state.vao);
	glBindVertexArray(game_state.vao);
	float vertices[]{
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.0f, 0.5f, 0.0f,
	};
	glGenBuffers(1, &game_state.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, game_state.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
}

void free_game_state(Game_State& game_state)
{
	glDeleteProgram(game_state.shader);

	glDeleteVertexArrays(1, &game_state.vao);
	glDeleteBuffers(1, &game_state.vbo);
}
