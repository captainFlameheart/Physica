#include <glad/glad.h>
#include "renderer.h"
#include "game_state.h"
#include <iostream>

void render(double const processed_time, Game_State const &game_state, GLFWwindow const *window, double lag)
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	
	glUseProgram(game_state.shader);
	glUniform1f(game_state.x_uniform, 0.1 * game_state.x);
	glBindVertexArray(game_state.vao);
	glDrawArrays(GL_TRIANGLES, 0, 3);
}
