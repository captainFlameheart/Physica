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
			util_shader_DEFINE("PROJECTION_SCALE", "vec2(0.5, 1.0)"),
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

			environment.state.camera_send_buffer = new unsigned char[environment.state.camera_buffer_size];

			glCreateBuffers(1, &environment.state.camera_buffer);
			glNamedBufferStorage
			(
				environment.state.camera_buffer, environment.state.camera_buffer_size, nullptr,
				GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT
			);
			glBindBufferBase(GL_UNIFORM_BUFFER, game_CAMERA_BINDING, environment.state.camera_buffer);

			GLenum const offset_label{ GL_OFFSET };

			GLuint const transform_index
			{
				glGetProgramResourceIndex(environment.state.shader, GL_UNIFORM, "Camera.transform")
			};
			glGetProgramResourceiv
			(
				environment.state.shader, GL_UNIFORM, transform_index,
				1, &offset_label, 1, nullptr, &environment.state.camera_buffer_transform_offset
			);

			GLuint const view_rotation_index
			{
				glGetProgramResourceIndex(environment.state.shader, GL_UNIFORM, "Camera.view_rotation")
			};
			glGetProgramResourceiv
			(
				environment.state.shader, GL_UNIFORM, view_rotation_index, 
				1, &offset_label, 1, nullptr, &environment.state.camera_buffer_view_rotation_offset
			);
			GLenum matrix_stride_label{ GL_MATRIX_STRIDE };
			glGetProgramResourceiv
			(
				environment.state.shader, GL_UNIFORM, view_rotation_index,
				1, &matrix_stride_label, 1, nullptr, &environment.state.camera_buffer_view_rotation_stride
			);
		}

		glGenVertexArrays(1, &environment.state.vao);
		glBindVertexArray(environment.state.vao);
		glGenBuffers(1, &environment.state.vbo);
		glBindBuffer(GL_ARRAY_BUFFER, environment.state.vbo);
		GLint vertices[]
		{
			game_FROM_METERS(environment, -1.0f), game_FROM_METERS(environment, -1.0f),
			0, game_FROM_METERS(environment, 1.0f),
			game_FROM_METERS(environment, 1.0f), game_FROM_METERS(environment, -1.0f),
		};
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glVertexAttribIPointer(0, 2, GL_INT, 2 * sizeof(GLint), static_cast<void const*>(0));
		glEnableVertexAttribArray(0);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		environment.state.camera.transform.xy.x = 0;
		environment.state.camera.transform.xy.y = 0;
		environment.state.camera.transform.z = game_FROM_METERS(environment, 2.0f);
		environment.state.camera.transform.angle = 0;
		environment.state.camera.view_rotation.column_0[0] = 1.0;
		environment.state.camera.view_rotation.column_0[1] = 0.0;
		environment.state.camera.view_rotation.column_1[0] = 0.0;
		environment.state.camera.view_rotation.column_1[1] = 1.0;
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
		GLint const fast_key_pressed{ glfwGetKey(environment.window, GLFW_KEY_LEFT_SHIFT) };
		GLint const slow_key_pressed{ glfwGetKey(environment.window, GLFW_KEY_LEFT_CONTROL) };
		GLfloat const distance
		{ 
			game_CAMERA_DEFAULT_DISTANCE_PER_TICK(environment) +
			fast_key_pressed * game_CAMERA_FAST_DISTANCE_PER_TICK_INCREASE(environment) - 
			slow_key_pressed * game_CAMERA_SLOW_DISTANCE_PER_TICK_DECREASE(environment) 
		};
		GLint const z_distance
		{ 
			game_CAMERA_DEFAULT_Z_DISTANCE_PER_TICK(environment) + 
			fast_key_pressed * game_CAMERA_FAST_Z_DISTANCE_PER_TICK_INCREASE(environment) - 
			slow_key_pressed * game_CAMERA_SLOW_Z_DISTANCE_PER_TICK_DECREASE(environment) 
		};
		GLint const angle
		{ 
			game_CAMERA_DEFAULT_ANGLE_PER_TICK(environment) + 
			fast_key_pressed * game_CAMERA_FAST_ANGLE_PER_TICK_INCREASE(environment) -
			slow_key_pressed * game_CAMERA_SLOW_ANGLE_PER_TICK_DECREASE(environment) 
		};

		GLint const x_sign
		{
			glfwGetKey(environment.window, GLFW_KEY_D) -
			glfwGetKey(environment.window, GLFW_KEY_A)
		};
		GLint const x_displacement_x
		{ 
			x_sign * 
			static_cast<GLint>(environment.state.camera.view_rotation.column_0[0] * distance) 
		};
		GLint const x_displacement_y
		{ 
			x_sign * 
			static_cast<GLint>(environment.state.camera.view_rotation.column_1[0] * distance)
		};
		environment.state.camera.transform.xy.x += x_displacement_x;
		environment.state.camera.transform.xy.y += x_displacement_y;

		GLint const y_sign
		{
			glfwGetKey(environment.window, GLFW_KEY_W) -
			glfwGetKey(environment.window, GLFW_KEY_S)
		};
		GLint const y_displacement_x
		{
			y_sign *
			static_cast<GLint>(environment.state.camera.view_rotation.column_0[1] * distance)
		};
		GLint const y_displacement_y
		{
			y_sign *
			static_cast<GLint>(environment.state.camera.view_rotation.column_1[1] * distance)
		};
		environment.state.camera.transform.xy.x += y_displacement_x;
		environment.state.camera.transform.xy.y += y_displacement_y;

		if (glfwGetKey(environment.window, GLFW_KEY_LEFT))
		{
			environment.state.camera.transform.angle += angle;
		}
		if (glfwGetKey(environment.window, GLFW_KEY_RIGHT))
		{
			environment.state.camera.transform.angle -= angle;
		}

		if (glfwGetKey(environment.window, GLFW_KEY_DOWN))
		{
			environment.state.camera.transform.z += z_distance;
		}
		if (glfwGetKey(environment.window, GLFW_KEY_UP))
		{
			environment.state.camera.transform.z -= z_distance;
		}

		// TODO: Separate into functions
		// TODO: Make sure to not loose precision due to large angles
		GLfloat const radians{ game_TO_RADIANS(environment, environment.state.camera.transform.angle) };
		GLfloat const right_x{ cos(radians) };
		GLfloat const right_y{ sin(radians) };
		environment.state.camera.view_rotation.column_0[0] = right_x;
		environment.state.camera.view_rotation.column_0[1] = -right_y;
		environment.state.camera.view_rotation.column_1[0] = right_y;
		environment.state.camera.view_rotation.column_1[1] = right_x;
	}

	void update_GPU_camera(game_environment::Environment& environment)
	{
		// TODO: Pass data directly to persistently mapped memory to avoid data copying
		// TODO: Store pointers instead of offsets to avoid additions
		std::memcpy
		(
			environment.state.camera_send_buffer + environment.state.camera_buffer_transform_offset, 
			&environment.state.camera.transform,
			sizeof(environment.state.camera.transform)
		);

		std::memcpy
		(
			environment.state.camera_send_buffer + environment.state.camera_buffer_view_rotation_offset,
			&environment.state.camera.view_rotation.column_0,
			sizeof(environment.state.camera.view_rotation.column_0)
		);

		std::memcpy
		(
			environment.state.camera_send_buffer + environment.state.camera_buffer_view_rotation_offset + environment.state.camera_buffer_view_rotation_stride,
			&environment.state.camera.view_rotation.column_1,
			sizeof(environment.state.camera.view_rotation.column_1)
		);

		glNamedBufferSubData
		(
			environment.state.camera_buffer, 0ll, environment.state.camera_buffer_size,
			environment.state.camera_send_buffer
		);

		// TODO: Check if memory barrier is needed to fix tearing
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
