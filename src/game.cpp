#include <glad/glad.h>
#include "game.h"
#include <GLFW/glfw3.h>
#include "util/shader.h"

void initialize_game_state(Full_Game_State &game_state)
{
	game_state.tick_delta_time = 1.0 / 90.0;
	game_state.max_ticks_per_frame = 10;

	//glGenBuffers(1, &game_state.state.projection_buffer);
	//glNamedBufferStorage(game_state.state.projection_buffer, )

	game_state.state.shader = util::shader::link_files("util/unique_world_position.vert", "util/static_color.frag");

	glGenVertexArrays(1, &game_state.state.vao);
	glBindVertexArray(game_state.state.vao);
	GLfloat vertices[]{
		-0.5f, -0.5f,
		0.5f, -0.5f,
		0.0f, 0.5f,
	};
	glGenBuffers(1, &game_state.state.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, game_state.state.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void on_key_event(
	Full_Game_State &game_state,
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
			glfwSetWindowShouldClose(game_state.window, GLFW_TRUE);
			break;
		case GLFW_KEY_F11:
			GLFWmonitor* monitor{ glfwGetPrimaryMonitor() };
			GLFWvidmode const* video_mode{ glfwGetVideoMode(monitor) };
			if (glfwGetWindowMonitor(game_state.window) == nullptr)
			{
				glfwSetWindowMonitor(game_state.window, monitor, 0, 0, video_mode->width, video_mode->height, video_mode->refreshRate);
			}
			else
			{
				int const width{ 640 }, height{ 320 };
				int const x{ (video_mode->width - width) / 2 }, y{ (video_mode->height - height) / 2 };
				glfwSetWindowMonitor(game_state.window, nullptr, x, y, width, height, GLFW_DONT_CARE);
			}
			break;
		}
	}
}

void on_cursor_event(
	Full_Game_State &game_state,
	double const x_pos,
	double const y_pos
)
{
	std::cout << x_pos << ", " << y_pos << std::endl;
}

void on_mouse_button_event(
	Full_Game_State &game_state,
	int const button,
	int const action,
	int const mods
)
{
	std::cout << button << std::endl;
}

void on_scroll_event(
	Full_Game_State &game_state,
	double const x_offset,
	double const y_offset
)
{
	std::cout << "(" << x_offset << ", " << y_offset << ")" << std::endl;
}

void tick(Full_Game_State &game_state)
{
}

void render(Full_Game_State &game_state)
{
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(game_state.state.shader);
	glBindVertexArray(game_state.state.vao);
	glDrawArrays(GL_TRIANGLES, 0, 3);
}

void free_game_state(Full_Game_State &game_state)
{
	glDeleteProgram(game_state.state.shader);

	glDeleteVertexArrays(1, &game_state.state.vao);
	glDeleteBuffers(1, &game_state.state.vbo);
}
