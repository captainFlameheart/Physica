#include <glad/glad.h>
#include "game/game.h"
#include <GLFW/glfw3.h>
#include "util/shader/shader.h"
#include <iostream>

namespace game
{
	void initialize(game_environment::Environment& environment)
	{
		//glGenBuffers(1, &game_state.state.projection_buffer);
		//glNamedBufferStorage(game_state.state.projection_buffer, )

		GLuint vertex_shader{ util::shader::create_shader(GL_VERTEX_SHADER) };
		GLuint fragment_shader{ util::shader::create_shader(GL_FRAGMENT_SHADER) };

		util::shader::set_shader_statically
		(
			vertex_shader,
			util_shader_VERSION,
			util::shader::file_to_string("util/unique_world_position.vert")
		);
		util::shader::set_shader_statically
		(
			fragment_shader,
			util_shader_VERSION,
			util_shader_DEFINE(COLOR vec4(0.0, 1.0, 0.0, 1.0)),
			util::shader::file_to_string("util/static_color.frag")
		);

		environment.state.shader = util::shader::create_program(vertex_shader, fragment_shader);

		util::shader::delete_shader(vertex_shader);
		util::shader::delete_shader(fragment_shader);

		glGenVertexArrays(1, &environment.state.vao);
		glBindVertexArray(environment.state.vao);
		GLfloat vertices[]{
			-0.5f, -0.5f,
			0.5f, -0.5f,
			0.0f, 0.5f,
		};
		glGenBuffers(1, &environment.state.vbo);
		glBindBuffer(GL_ARRAY_BUFFER, environment.state.vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*)0);
		glEnableVertexAttribArray(0);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	}

	void on_key_event(
		game_environment::Environment& environment,
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
				glfwSetWindowShouldClose(environment.window, GLFW_TRUE);
				break;
			case GLFW_KEY_F11:
				GLFWmonitor* monitor{ glfwGetPrimaryMonitor() };
				GLFWvidmode const* video_mode{ glfwGetVideoMode(monitor) };
				if (glfwGetWindowMonitor(environment.window) == nullptr)
				{
					glfwSetWindowMonitor(environment.window, monitor, 0, 0, video_mode->width, video_mode->height, video_mode->refreshRate);
				}
				else
				{
					int const width{ 640 }, height{ 320 };
					int const x{ (video_mode->width - width) / 2 }, y{ (video_mode->height - height) / 2 };
					glfwSetWindowMonitor(environment.window, nullptr, x, y, width, height, GLFW_DONT_CARE);
				}
				break;
			}
		}
	}

	void on_cursor_event(
		game_environment::Environment& environment,
		double const x_pos,
		double const y_pos
	)
	{
		std::cout << x_pos << ", " << y_pos << std::endl;
	}

	void on_mouse_button_event(
		game_environment::Environment& environment,
		int const button,
		int const action,
		int const mods
	)
	{
		std::cout << button << std::endl;
	}

	void on_scroll_event(
		game_environment::Environment& environment,
		double const x_offset,
		double const y_offset
	)
	{
		std::cout << "(" << x_offset << ", " << y_offset << ")" << std::endl;
	}

	void tick(game_environment::Environment& environment)
	{
		std::cout << "Tick" << std::endl;
	}

	void render(game_environment::Environment& environment)
	{
		std::cout << "Render" << std::endl;

		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(environment.state.shader);
		glBindVertexArray(environment.state.vao);
		glDrawArrays(GL_TRIANGLES, 0, 3);
	}

	void free(game_environment::Environment& environment)
	{
		util::shader::delete_program(environment.state.shader);

		glDeleteVertexArrays(1, &environment.state.vao);
		glDeleteBuffers(1, &environment.state.vbo);
	}
}
