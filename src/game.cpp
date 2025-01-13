#include <glad/glad.h>
#include "game.h"
#include <GLFW/glfw3.h>
#include "util/shader.h"

void initialize_game_state(
	GLFWwindow* window,
	double &tick_delta_time, 
	unsigned &max_ticks_per_frame, 
	Game_State &game_state
)
{
	tick_delta_time = 1.0 / 90.0;
	max_ticks_per_frame = 10;

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
)
{
	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, GLFW_TRUE);
			break;
		case GLFW_KEY_F11:
			GLFWmonitor* monitor{ glfwGetPrimaryMonitor() };
			GLFWvidmode const* video_mode{ glfwGetVideoMode(monitor) };
			if (glfwGetWindowMonitor(window) == nullptr)
			{
				glfwSetWindowMonitor(window, monitor, 0, 0, video_mode->width, video_mode->height, video_mode->refreshRate);
			}
			else
			{
				int const width{ 640 }, height{ 320 };
				int const x{ (video_mode->width - width) / 2 }, y{ (video_mode->height - height) / 2 };
				glfwSetWindowMonitor(window, nullptr, x, y, width, height, GLFW_DONT_CARE);
			}
			break;
		}
	}
}

void on_cursor_event(
	GLFWwindow *window,
	double const tick_delta_time,
	unsigned const max_ticks_per_frame,
	Game_State &game_state,
	double const processed_time,
	double const x_pos,
	double const y_pos
)
{
	std::cout << x_pos << ", " << y_pos << std::endl;
}

void on_mouse_button_event(
	GLFWwindow *window,
	double const tick_delta_time,
	unsigned const max_ticks_per_frame,
	Game_State &game_state,
	double const processed_time,
	int const button,
	int const action,
	int const mods
)
{
	std::cout << button << std::endl;
}

void on_scroll_event(
	GLFWwindow *window,
	double const tick_delta_time,
	unsigned const max_ticks_per_frame,
	Game_State &game_state,
	double const processed_time,
	double const x_offset,
	double const y_offset
)
{
	std::cout << "(" << x_offset << ", " << y_offset << ")" << std::endl;
}

void tick(
	GLFWwindow* window,
	double const delta_time,
	unsigned const max_ticks_per_frame,
	Game_State &game_state,
	double const processed_time,
	unsigned const ticks_this_frame
)
{
	game_state.x += 1.0 * delta_time;
}

void render(
	GLFWwindow const *window,
	double const tick_delta_time,
	unsigned const max_ticks_per_frame,
	Game_State const &game_state,
	double const processed_time,
	double lag
)
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(game_state.shader);
	glUniform1f(game_state.x_uniform, 0.1 * game_state.x);
	glBindVertexArray(game_state.vao);
	glDrawArrays(GL_TRIANGLES, 0, 3);
}

void free_game_state(
	GLFWwindow *window,
	double const tick_delta_time,
	unsigned const max_ticks_per_frame,
	Game_State &game_state,
	double const processed_time
)
{
	glDeleteProgram(game_state.shader);

	glDeleteVertexArrays(1, &game_state.vao);
	glDeleteBuffers(1, &game_state.vbo);
}
