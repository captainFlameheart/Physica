#include <glad/glad.h>
#include "game/game.h"
#include <GLFW/glfw3.h>
#include "util/shader/shader.h"
#include <iostream>
#include "macros/macros.h"

namespace game
{
	void initialize(game_environment::Environment& environment)
	{
		GLuint vertex_shader{ util::shader::create_shader(GL_VERTEX_SHADER) };
		GLuint fragment_shader{ util::shader::create_shader(GL_FRAGMENT_SHADER) };
		util::shader::set_shader_statically
		(
			vertex_shader,
			util_shader_VERSION,
			util_shader_DEFINE("PROJECTION_SCALE", "vec2(1.0, 1.0)"),
			util_shader_DEFINE("CAMERA_BINDING", STRINGIFY(game_CAMERA_BINDING)),
			util::shader::file_to_string("util/unique_world_position.vert")
		);
		util::shader::set_shader_statically
		(
			fragment_shader,
			util_shader_VERSION,
			util_shader_DEFINE("COLOR", "vec4(0.0, 1.0, 0.0, 1.0)"),
			util::shader::file_to_string("util/static_color.frag")
		);

		environment.state.shader = util::shader::create_program(vertex_shader, fragment_shader);

		util::shader::delete_shader(vertex_shader);
		util::shader::delete_shader(fragment_shader);

		{
			GLuint const block_index
			{
				glGetProgramResourceIndex(environment.state.shader, GL_UNIFORM_BLOCK, "Camera")
			};
			GLenum const buffer_size_label{ GL_BUFFER_DATA_SIZE };
			glGetProgramResourceiv
			(
				environment.state.shader, GL_UNIFORM_BLOCK, block_index, 
				1, &buffer_size_label, 1, nullptr, &environment.state.camera_buffer_size
			);
			std::cout << environment.state.camera_buffer_size << std::endl;

			environment.state.camera_send_buffer = new unsigned char[environment.state.camera_buffer_size];

			glCreateBuffers(1, &environment.state.camera_buffer);
			glNamedBufferStorage
			(
				environment.state.camera_buffer, environment.state.camera_buffer_size, nullptr,
				GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT
			);
			glBindBufferBase(GL_UNIFORM_BUFFER, game_CAMERA_BINDING, environment.state.camera_buffer);

			GLenum const offset_label{ GL_OFFSET };

			GLuint const xy_index
			{
				glGetProgramResourceIndex(environment.state.shader, GL_UNIFORM, "Camera.xy")
			};
			glGetProgramResourceiv
			(
				environment.state.shader, GL_UNIFORM, xy_index, 
				1, &offset_label, 1, nullptr, &environment.state.camera_buffer_xy_offset
			);

			GLuint const right_index
			{
				glGetProgramResourceIndex(environment.state.shader, GL_UNIFORM, "Camera.right")
			};
			glGetProgramResourceiv
			(
				environment.state.shader, GL_UNIFORM, right_index, 
				1, &offset_label, 1, nullptr, &environment.state.camera_buffer_right_offset
			);

			GLuint const z_index
			{
				glGetProgramResourceIndex(environment.state.shader, GL_UNIFORM, "Camera.z")
			};
			glGetProgramResourceiv
			(
				environment.state.shader, GL_UNIFORM, z_index, 
				1, &offset_label, 1, nullptr, &environment.state.camera_buffer_z_offset
			);

			std::cout << environment.state.camera_buffer_xy_offset << std::endl;
			std::cout << environment.state.camera_buffer_right_offset << std::endl;
			std::cout << environment.state.camera_buffer_z_offset << std::endl;
		}

		glGenVertexArrays(1, &environment.state.vao);
		glBindVertexArray(environment.state.vao);
		glGenBuffers(1, &environment.state.vbo);
		glBindBuffer(GL_ARRAY_BUFFER, environment.state.vbo);
		GLint vertices[]
		{
			-2, -1,
			0, 1,
			1, -1,
		};
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glVertexAttribIPointer(0, 2, GL_INT, 2 * sizeof(GLint), static_cast<void const*>(0));
		glEnableVertexAttribArray(0);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		environment.state.camera.xy.x = 0;
		environment.state.camera.xy.y = 0;
		environment.state.camera.angle = 0;
		environment.state.camera.z = game_FROM_METERS(environment, 2.0);
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
		// v * dt
		// 
		//environment.state.camera.xy.x = game_FROM_METERS(environment, 0.1);
	}

	void update_GPU_camera(game_environment::Environment& environment)
	{
		// TODO: Pass data directly to persistently mapped memory to avoid data copying
		// TODO: Store pointers instead of offsets to avoid additions
		std::cout << environment.state.camera.xy << std::endl;
		std::memcpy
		(
			environment.state.camera_send_buffer + environment.state.camera_buffer_xy_offset, 
			&environment.state.camera.xy,
			2 * sizeof(GLfloat)
		);

		// TODO: Separate into functions
		double radians{game_TO_RADIANS(environment, environment.state.camera.angle)};
		util::math::Vector_2D right
		{ 
			game_FROM_METERS(environment, cos(radians)), 
			game_FROM_METERS(environment, sin(radians)) 
		};

		std::memcpy
		(
			environment.state.camera_send_buffer + environment.state.camera_buffer_right_offset,
			&right,
			sizeof(util::math::Vector_2D)
		);

		std::memcpy
		(
			environment.state.camera_send_buffer + environment.state.camera_buffer_z_offset,
			&environment.state.camera.z,
			sizeof(GLfloat)
		);
		
		glNamedBufferSubData
		(
			environment.state.camera_buffer, 0ll, environment.state.camera_buffer_size,
			environment.state.camera_send_buffer
		);
	}

	void render(game_environment::Environment& environment)
	{
		glClear(GL_COLOR_BUFFER_BIT);

		update_GPU_camera(environment);

		glUseProgram(environment.state.shader);
		glBindVertexArray(environment.state.vao);
		glDrawArrays(GL_TRIANGLES, 0, 3);
	}

	void free(game_environment::Environment& environment)
	{
		util::shader::delete_program(environment.state.shader);

		delete[] environment.state.camera_send_buffer;
		glDeleteBuffers(1, &environment.state.camera_buffer);

		glDeleteVertexArrays(1, &environment.state.vao);
		glDeleteBuffers(1, &environment.state.vbo);
	}
}
