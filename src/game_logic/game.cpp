#include "glad_glfw.h"
#include "game_logic/game.h"
#include "util/shader/shader.h"
#include <iostream>
#include <iterator>
#include "macros/macros.h"
#include "game_logic/util/glfw/window_screen_size.h"
#include "game_logic/window_to_camera/window_to_camera.h"
#include "game_logic/window_to_world/window_to_world.h"
#include "game_logic/util/spatial/FLOAT_FROM_METERS.h"
#include "game_logic/util/spatial/FROM_METERS.h"
#include "game_logic/util/spatial/TO_RADIANS.h"
#include "game_logic/util/camera/unit_z_vector_to_camera_local_world_vector.h"
#include "game_logic/util/camera/increase_camera_angle.h"
#include "game_logic/util/camera/local_world_vector_to_world_vector.h"
#include "game_logic/util/camera/local_world_position_to_world_position.h"
#include "game_logic/util/rigid_body/VELOCITY_INTEGRATION_LOCAL_SIZE.h"
#include "game_logic/util/rigid_body/Position.h"
#include "game_logic/util/rigid_body/Velocity.h"
#include "game_logic/util/rigid_body/Triangle.h"

// TODO: Reduce by a magnitude of 10
#define game_logic_MAX_RIGID_BODY_COUNT(environment) \
	10000u * game_logic__util__rigid_body_VELOCITY_INTEGRATION_LOCAL_SIZE(environment)

#define game_logic_MAX_TRIANGLE_COUNT(environment) \
	game_logic_MAX_RIGID_BODY_COUNT(environment)

#define game_logic_MAX_VERTEX_COUNT(environment) \
	3u * game_logic_MAX_TRIANGLE_COUNT(environment)

namespace game_logic
{
	void initialize(game_environment::Environment& environment)
	{
		environment.state.grab_cursor = glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);

		environment.state.camera.xy.x = 0;
		environment.state.camera.xy.y = 0;
		environment.state.camera.angle = 0;
		environment.state.camera.z = game_logic__util__spatial_FLOAT_FROM_METERS(environment, 2.0f);
		environment.state.camera.view_rotation.column_0[0] = 1.0f;
		environment.state.camera.view_rotation.column_0[1] = 0.0f;
		environment.state.camera.view_rotation.column_1[0] = 0.0f;
		environment.state.camera.view_rotation.column_1[1] = 1.0f;
		
		environment.state.point_grabbed = false;

		GLuint const vertex_shader{ ::util::shader::create_shader(GL_VERTEX_SHADER) };
		GLuint const fragment_shader{ ::util::shader::create_shader(GL_FRAGMENT_SHADER) };
		GLuint const compute_shader{ ::util::shader::create_shader(GL_COMPUTE_SHADER) };

		::util::shader::set_shader_statically
		(
			vertex_shader,
			util_shader_VERSION,
			game_PROJECTION_SCALE_DEFINITION(environment),
			util_shader_DEFINE("CAMERA_BINDING", STRINGIFY(game_CAMERA_BINDING)),
			::util::shader::file_to_string("util/unique_world_position.vert")
		);
		::util::shader::set_shader_statically
		(
			fragment_shader,
			util_shader_VERSION,
			util_shader_DEFINE("COLOR", "vec4(0.0, 1.0, 0.0, 1.0)"),
			::util::shader::file_to_string("util/static_color.frag")
		);
		environment.state.shader = ::util::shader::create_program(vertex_shader, fragment_shader);

		::util::shader::set_shader_statically
		(
			vertex_shader,
			util_shader_VERSION,
			game_PROJECTION_SCALE_DEFINITION(environment),
			util_shader_DEFINE("CAMERA_BINDING", STRINGIFY(game_CAMERA_BINDING)),
			util_shader_DEFINE("POSITION_BINDING", STRINGIFY(game_logic__util_RIGID_BODY_POSITION_BINDING)),
			util_shader_DEFINE("METER", STRINGIFY(game_logic__util__spatial_METER(environment))), 
			util_shader_DEFINE("RADIAN_INVERSE", STRINGIFY(game_logic__util__spatial_RADIAN_INVERSE(environment))), 
			::util::shader::file_to_string("util/rigid_body_debug.vert")
		);
		// TODO: REMOVE
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//glEnable(GL_BLEND);
		::util::shader::set_shader_statically
		(
			fragment_shader,
			util_shader_VERSION,
			util_shader_DEFINE("COLOR", "vec4(1.0, 1.0, 0.0, 1.0)"), 
			::util::shader::file_to_string("util/static_color.frag")
		);
		environment.state.rigid_body_debug_rendering_shader = ::util::shader::create_program(vertex_shader, fragment_shader);

		::util::shader::set_shader_statically
		(
			vertex_shader,
			util_shader_VERSION,
			game_PROJECTION_SCALE_DEFINITION(environment),
			util_shader_DEFINE("CAMERA_BINDING", STRINGIFY(game_CAMERA_BINDING)),
			util_shader_DEFINE("POSITION_BINDING", STRINGIFY(game_logic__util_RIGID_BODY_POSITION_BINDING)),
			util_shader_DEFINE("TRIANGLE_BINDING", STRINGIFY(game_logic__util_TRIANGLE_BINDING)),
			util_shader_DEFINE("VERTEX_BINDING", STRINGIFY(game_logic__util_VERTEX_BINDING)),
			util_shader_DEFINE("METER", STRINGIFY(game_logic__util__spatial_METER(environment))), // TODO: Remove
			util_shader_DEFINE("RADIAN_INVERSE", STRINGIFY(game_logic__util__spatial_RADIAN_INVERSE(environment))),
			::util::shader::file_to_string("util/triangle.vert")
		);
		::util::shader::set_shader_statically
		(
			fragment_shader,
			util_shader_VERSION,
			util_shader_DEFINE("COLOR", "vec4(1.0, 0.0, 1.0, 1.0)"),
			::util::shader::file_to_string("util/static_color.frag") // TODO: Should only be done once
		);
		environment.state.triangle_draw_shader = ::util::shader::create_program(vertex_shader, fragment_shader);

		::util::shader::set_shader_statically
		(
			compute_shader,
			util_shader_VERSION,
			util_shader_DEFINE("POSITION_BINDING", STRINGIFY(game_logic__util_RIGID_BODY_POSITION_BINDING)),
			util_shader_DEFINE("VELOCITY_BINDING", STRINGIFY(game_logic__util_RIGID_BODY_VELOCITY_BINDING)), 
			util_shader_DEFINE("LOCAL_SIZE", STRINGIFY(game_logic__util__rigid_body_VELOCITY_INTEGRATION_LOCAL_SIZE(environment))), 
			::util::shader::file_to_string("util/rigid_body_velocity_integration.comp")
		);
		environment.state.rigid_body_velocity_integration_shader = ::util::shader::create_program(compute_shader);

		::util::shader::delete_shader(vertex_shader);
		::util::shader::delete_shader(fragment_shader);
		::util::shader::delete_shader(compute_shader);

		// TODO: Consider putting buffers next to each other in game state
		// TODO: Probably flip position of position and velocity buffer
		GLuint buffers[]
		{ 
			environment.state.camera_buffer, 
			environment.state.rigid_body_position_buffer, 
			environment.state.rigid_body_velocity_buffer, 
			environment.state.triangle_buffer, 
			environment.state.vertex_buffer
		};
		glCreateBuffers(std::size(buffers), buffers);
		environment.state.camera_buffer = buffers[0];
		environment.state.rigid_body_position_buffer = buffers[1];
		environment.state.rigid_body_velocity_buffer = buffers[2];
		environment.state.triangle_buffer = buffers[3];
		environment.state.vertex_buffer = buffers[4];

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

			// TODO: Remove GL_DYNAMIC_STORAGE_BIT when persistent mapping 
			// is used to upload data instead of glSubData
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

			GLuint const angle_index
			{
				glGetProgramResourceIndex(environment.state.shader, GL_UNIFORM, "Camera.angle")
			};
			glGetProgramResourceiv
			(
				environment.state.shader, GL_UNIFORM, angle_index,
				1, &offset_label, 1, nullptr, &environment.state.camera_buffer_angle_offset
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

			GLuint const view_rotation_index
			{
				glGetProgramResourceIndex(environment.state.shader, GL_UNIFORM, "Camera.view_rotation")
			};
			glGetProgramResourceiv
			(
				environment.state.shader, GL_UNIFORM, view_rotation_index, 
				1, &offset_label, 1, nullptr, &environment.state.camera_buffer_view_rotation_offset
			);
			// TODO: Make this query part of the above call
			GLenum matrix_stride_label{ GL_MATRIX_STRIDE };
			glGetProgramResourceiv
			(
				environment.state.shader, GL_UNIFORM, view_rotation_index,
				1, &matrix_stride_label, 1, nullptr, &environment.state.camera_buffer_view_rotation_stride
			);
		}

		environment.state.current_rigid_body_count = 100000u;
		environment.state.current_triangle_count = 2u * environment.state.current_rigid_body_count;

		{ // Position buffer
			GLuint const p_index
			{
				glGetProgramResourceIndex(environment.state.rigid_body_velocity_integration_shader, GL_BUFFER_VARIABLE, "Positions.p")
			};

			GLenum const prop_labels[]{ GL_OFFSET, GL_ARRAY_STRIDE };
			GLint props[std::size(prop_labels)];
			glGetProgramResourceiv
			(
				environment.state.rigid_body_velocity_integration_shader, GL_BUFFER_VARIABLE, p_index,
				std::size(prop_labels), prop_labels, 2, nullptr, props
			);
			// TODO: Consider putting p_offset and p_stride contigously in game state
			environment.state.rigid_body_position_buffer_p_offset = props[0];
			environment.state.rigid_body_position_buffer_p_stride = props[1];

			environment.state.rigid_body_position_buffer_size =
			(
				environment.state.rigid_body_position_buffer_p_offset + 
				game_logic_MAX_RIGID_BODY_COUNT(environment) * 
				environment.state.rigid_body_position_buffer_p_stride
			);
			// TODO: Don't initialize a few positions by copying over the ENTIRE buffer 
			// content from CPU to GPU like this. Instead, use persistent mapping 
			// for both initialization and updating.
			unsigned char* const initial_positions = new unsigned char[environment.state.rigid_body_position_buffer_size];
			
			for (GLuint i = 0; i < environment.state.current_rigid_body_count; ++i)
			{
				GLuint const width{ 316u };
				util::rigid_body::Position position
				{
					{
						game_logic__util__spatial_FROM_METERS(environment, (i % width) * 0.5f),
						game_logic__util__spatial_FROM_METERS(environment, (i / width) * 0.5f)
					}, 
					game_logic__util__spatial_FROM_RADIANS(environment, i * 0.1f)
				};
				std::memcpy
				(
					initial_positions + environment.state.rigid_body_position_buffer_p_offset + i * environment.state.rigid_body_position_buffer_p_stride,
					&position, sizeof(position)
				);
			}
			/*position.position.x = game_logic__util__spatial_FROM_METERS(environment, -1.0f);
			position.position.y = 0;
			position.angle = 0;
			std::memcpy
			(
				initial_positions + environment.state.rigid_body_position_buffer_p_offset + environment.state.rigid_body_position_buffer_p_stride,
				&position, sizeof(position)
			);*/

			glNamedBufferStorage
			(
				environment.state.rigid_body_position_buffer, environment.state.rigid_body_position_buffer_size, initial_positions,
				GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT
			);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, game_logic__util_RIGID_BODY_POSITION_BINDING, environment.state.rigid_body_position_buffer);

			delete[] initial_positions;
		}

		{ // Velocity buffer
			GLuint const v_index
			{
				glGetProgramResourceIndex(environment.state.rigid_body_velocity_integration_shader, GL_BUFFER_VARIABLE, "Velocities.v")
			};

			GLenum const prop_labels[]{ GL_OFFSET, GL_ARRAY_STRIDE };
			GLint props[std::size(prop_labels)];
			glGetProgramResourceiv
			(
				environment.state.rigid_body_velocity_integration_shader, GL_BUFFER_VARIABLE, v_index,
				std::size(prop_labels), prop_labels, 2, nullptr, props
			);
			// TODO: Consider putting v_offset and v_stride contigously in game state
			environment.state.rigid_body_velocity_buffer_v_offset = props[0];
			environment.state.rigid_body_velocity_buffer_v_stride = props[1];

			environment.state.rigid_body_velocity_buffer_size =
			(
				environment.state.rigid_body_velocity_buffer_v_offset +
				game_logic_MAX_RIGID_BODY_COUNT(environment) *
				environment.state.rigid_body_velocity_buffer_v_stride
			);
			// TODO: Don't initialize a few positions by copying over the ENTIRE buffer 
			// content from CPU to GPU like this. Instead, use persistent mapping 
			// for both initialization and updating.
			unsigned char* const initial_velocities = new unsigned char[environment.state.rigid_body_velocity_buffer_size];

			for (GLuint i = 0; i < environment.state.current_rigid_body_count; ++i)
			{
				util::rigid_body::Velocity velocity
				{
					{
						game_METERS_PER_SECOND_TO_LENGTH_PER_TICK(environment, -sin(i * 0.1f)),
						game_METERS_PER_SECOND_TO_LENGTH_PER_TICK(environment, cos(i * 0.1f))
					},
					game_RADIANS_PER_SECOND_TO_ANGLE_PER_TICK(environment, 0.5f)
				};
				std::memcpy
				(
					initial_velocities + environment.state.rigid_body_velocity_buffer_v_offset + i * environment.state.rigid_body_velocity_buffer_v_stride, 
					&velocity, sizeof(velocity)
				);
			}

			glNamedBufferStorage
			(
				environment.state.rigid_body_velocity_buffer, environment.state.rigid_body_velocity_buffer_size, initial_velocities,
				GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT
			);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, game_logic__util_RIGID_BODY_VELOCITY_BINDING, environment.state.rigid_body_velocity_buffer);

			delete[] initial_velocities;
		}

		{ // Triangle buffer
			GLuint const triangles_index
			{
				glGetProgramResourceIndex(environment.state.triangle_draw_shader, GL_BUFFER_VARIABLE, "Triangles.triangles")
			};

			GLenum const prop_labels[]{ GL_OFFSET, GL_ARRAY_STRIDE };
			GLint props[std::size(prop_labels)];
			glGetProgramResourceiv
			(
				environment.state.triangle_draw_shader, GL_BUFFER_VARIABLE, triangles_index,
				std::size(prop_labels), prop_labels, 2, nullptr, props
			);
			// TODO: Consider putting offset and stride contigously in game state
			environment.state.triangle_buffer_triangles_offset = props[0];
			environment.state.triangle_buffer_triangles_stride = props[1];

			environment.state.triangle_buffer_size =
			(
				environment.state.triangle_buffer_triangles_offset +
				game_logic_MAX_TRIANGLE_COUNT(environment) *
				environment.state.triangle_buffer_triangles_stride
			);
			// TODO: Don't initialize a few triangles by copying over the ENTIRE buffer 
			// content from CPU to GPU like this. Instead, use persistent mapping 
			// for both initialization and updating.
			unsigned char* const initial_triangles = new unsigned char[environment.state.triangle_buffer_size];

			for (GLuint i = 0; i < environment.state.current_triangle_count; ++i)
			{
				util::rigid_body::Triangle triangle
				{
					{(2 * i) % 4, (2 * i + 1) % 4, (2 * i + 2) % 4}, i / 2
				};
				std::memcpy
				(
					initial_triangles + environment.state.triangle_buffer_triangles_offset + i * environment.state.triangle_buffer_triangles_stride,
					&triangle, sizeof(triangle)
				);
			}

			glNamedBufferStorage
			(
				environment.state.triangle_buffer, environment.state.triangle_buffer_size, initial_triangles,
				GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT
			);

			delete[] initial_triangles;

			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, game_logic__util_TRIANGLE_BINDING, environment.state.triangle_buffer);
		}

		{ // Vertex buffer
			GLuint const vertices_index
			{
				glGetProgramResourceIndex(environment.state.triangle_draw_shader, GL_BUFFER_VARIABLE, "Vertices.vertices")
			};

			GLenum const prop_labels[]{ GL_OFFSET, GL_ARRAY_STRIDE };
			GLint props[std::size(prop_labels)];
			glGetProgramResourceiv
			(
				environment.state.triangle_draw_shader, GL_BUFFER_VARIABLE, vertices_index,
				std::size(prop_labels), prop_labels, 2, nullptr, props
			);
			// TODO: Consider putting offset and stride contigously in game state
			environment.state.vertex_buffer_vertices_offset = props[0];
			environment.state.vertex_buffer_vertices_stride = props[1];

			environment.state.vertex_buffer_size =
			(
				environment.state.vertex_buffer_vertices_offset +
				game_logic_MAX_VERTEX_COUNT(environment) *
				environment.state.vertex_buffer_vertices_stride
			);
			// TODO: Don't initialize a few vertices by copying over the ENTIRE buffer 
			// content from CPU to GPU like this. Instead, use persistent mapping 
			// for both initialization and updating.
			unsigned char* const initial_vertices = new unsigned char[environment.state.vertex_buffer_size];
			GLfloat vertex[2];
			GLfloat const r = 0.5f;
			vertex[0] = game_logic__util__spatial_FLOAT_FROM_METERS(environment, r);
			vertex[1] = game_logic__util__spatial_FLOAT_FROM_METERS(environment, r);
			std::memcpy
			(
				initial_vertices + environment.state.vertex_buffer_vertices_offset + 0 * environment.state.vertex_buffer_vertices_stride,
				&vertex, sizeof(vertex)
			);

			vertex[0] = game_logic__util__spatial_FLOAT_FROM_METERS(environment, -r);
			vertex[1] = game_logic__util__spatial_FLOAT_FROM_METERS(environment, r);
			std::memcpy
			(
				initial_vertices + environment.state.vertex_buffer_vertices_offset + 1 * environment.state.vertex_buffer_vertices_stride,
				&vertex, sizeof(vertex)
			);

			vertex[0] = game_logic__util__spatial_FLOAT_FROM_METERS(environment, -r);
			vertex[1] = game_logic__util__spatial_FLOAT_FROM_METERS(environment, -r);
			std::memcpy
			(
				initial_vertices + environment.state.vertex_buffer_vertices_offset + 2 * environment.state.vertex_buffer_vertices_stride,
				&vertex, sizeof(vertex)
			);

			vertex[0] = game_logic__util__spatial_FLOAT_FROM_METERS(environment, r);
			vertex[1] = game_logic__util__spatial_FLOAT_FROM_METERS(environment, -r);
			std::memcpy
			(
				initial_vertices + environment.state.vertex_buffer_vertices_offset + 3 * environment.state.vertex_buffer_vertices_stride,
				&vertex, sizeof(vertex)
			);
			/*for (GLuint i = 0; i < environment.state.current_triangle_count; ++i)
			{
				util::rigid_body::Triangle triangle
				{
					{0u, 1u, 2u}, i
				};
				std::memcpy
				(
					initial_triangles + environment.state.triangle_buffer_triangles_offset + i * environment.state.triangle_buffer_triangles_stride,
					&triangle, sizeof(triangle)
				);
			}*/

			glNamedBufferStorage
			(
				environment.state.vertex_buffer, environment.state.vertex_buffer_size, initial_vertices,
				GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT
			);

			delete[] initial_vertices;

			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, game_logic__util_VERTEX_BINDING, environment.state.vertex_buffer);
		}

		glGenVertexArrays(1, &environment.state.vao);
		glBindVertexArray(environment.state.vao);
		glGenBuffers(1, &environment.state.vbo);
		glBindBuffer(GL_ARRAY_BUFFER, environment.state.vbo);
		GLint vertices[]
		{
			game_logic__util__spatial_FROM_METERS(environment, -1.0f), game_logic__util__spatial_FROM_METERS(environment, -1.0f),
			0, game_logic__util__spatial_FROM_METERS(environment, 1.0f),
			game_logic__util__spatial_FROM_METERS(environment, 1.0f), game_logic__util__spatial_FROM_METERS(environment, -1.0f),

			game_logic__util__spatial_FROM_METERS(environment, 2.0f), game_logic__util__spatial_FROM_METERS(environment, 0.0f),
			game_logic__util__spatial_FROM_METERS(environment, 3.0f), game_logic__util__spatial_FROM_METERS(environment, 1.0f),
			game_logic__util__spatial_FROM_METERS(environment, 3.5f), game_logic__util__spatial_FROM_METERS(environment, -0.5f),
		};
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glVertexAttribIPointer(0, 2, GL_INT, 2 * sizeof(GLint), static_cast<void const*>(0));
		glEnableVertexAttribArray(0);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		std::cout << "Position buffer (" << environment.state.rigid_body_position_buffer << "):" << std::endl;
		std::cout << "size: " << environment.state.rigid_body_position_buffer_size << std::endl;
		std::cout << "p offset: " << environment.state.rigid_body_position_buffer_p_offset << std::endl;
		std::cout << "p stride: " << environment.state.rigid_body_position_buffer_p_stride << std::endl;
		std::cout << std::endl;

		std::cout << "Velocity buffer (" << environment.state.rigid_body_velocity_buffer << "):" << std::endl;
		std::cout << "size: " << environment.state.rigid_body_velocity_buffer_size << std::endl;
		std::cout << "v offset: " << environment.state.rigid_body_velocity_buffer_v_offset << std::endl;
		std::cout << "v stride: " << environment.state.rigid_body_velocity_buffer_v_stride << std::endl;
		std::cout << std::endl;

		std::cout << "Triangle buffer (" << environment.state.triangle_buffer << "):" << std::endl;
		std::cout << "size: " << environment.state.triangle_buffer_size << std::endl;
		std::cout << "triangles offset: " << environment.state.triangle_buffer_triangles_offset << std::endl;
		std::cout << "triangles stride: " << environment.state.triangle_buffer_triangles_stride << std::endl;
		std::cout << std::endl;

		std::cout << "Vertex buffer: (" << environment.state.vertex_buffer << "):" << std::endl;
		std::cout << "size: " << environment.state.vertex_buffer_size << std::endl;
		std::cout << "vertices offset: " << environment.state.vertex_buffer_vertices_offset << std::endl;
		std::cout << "vertices stride: " << environment.state.vertex_buffer_vertices_stride << std::endl;
		std::cout << std::endl;
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
	}

	void on_mouse_button_event(
		game_environment::Environment& environment,
		int const button,
		int const action,
		int const mods
	)
	{
		switch (button)
		{
		case GLFW_MOUSE_BUTTON_RIGHT:
			switch (action)
			{ 
			case GLFW_PRESS:
				environment.state.point_grabbed = true;
				window_to_world::window_screen_cursor_position_to_world_position(
					environment,
					&environment.state.grabbed_point.x, &environment.state.grabbed_point.y
				);
				glfwSetCursor(environment.window, environment.state.grab_cursor);
				break; 
			case GLFW_RELEASE:
				environment.state.point_grabbed = false;
				glfwSetCursor(environment.window, nullptr);
				break;
			}
			break;
		}
	}

	void on_scroll_event(
		game_environment::Environment& environment,
		double const x_offset,
		double const y_offset
	)
	{
		double const horizontal_scroll_enabled
		{ 
			static_cast<double>
			(
				glfwGetKey(environment.window, GLFW_KEY_LEFT_SHIFT)
			) 
		};
		GLfloat const zoom_scroll{ 
			static_cast<GLfloat>
			(
				(1.0 - horizontal_scroll_enabled) * y_offset
			) 
		};
		GLfloat const zoom_scroll_distance
		{ 
			zoom_scroll * game_CAMERA_SCROLL_ZOOM_DISTANCE(environment) 
		};
		GLfloat camera_local_cursor_unit_z_x, camera_local_cursor_unit_z_y;
		window_to_camera::window_screen_cursor_position_to_camera_local_unit_z_vector
		(
			environment, 
			&camera_local_cursor_unit_z_x, &camera_local_cursor_unit_z_y
		);
		GLfloat const delta_z{ 
			zoom_scroll_distance / 
			sqrtf
			(
				camera_local_cursor_unit_z_x * camera_local_cursor_unit_z_x +
				camera_local_cursor_unit_z_y * camera_local_cursor_unit_z_y +
				1.0f
			) 
		};
		
		GLfloat camera_local_world_cursor_x, camera_local_world_cursor_y;
		util::camera::unit_z_vector_to_camera_local_world_vector
		(
			environment, 
			camera_local_cursor_unit_z_x, camera_local_cursor_unit_z_y, 
			&camera_local_world_cursor_x, &camera_local_world_cursor_y
		);
		GLint world_cursor_x, world_cursor_y;
		util::camera::local_world_position_to_world_position
		(
			environment, 
			camera_local_world_cursor_x, camera_local_world_cursor_y, 
			&world_cursor_x, &world_cursor_y
		);

		environment.state.camera.z -= delta_z;

		GLfloat const angle_scroll
		{
			static_cast<GLfloat>
			(
				x_offset + horizontal_scroll_enabled * y_offset
			)
		};
		GLfloat const delta_angle{ angle_scroll * game_CAMERA_SCROLL_ANGLE(environment) };
		util::camera::increase_camera_angle(environment, delta_angle);

		util::camera::unit_z_vector_to_camera_local_world_vector
		(
			environment,
			camera_local_cursor_unit_z_x, camera_local_cursor_unit_z_y,
			&camera_local_world_cursor_x, &camera_local_world_cursor_y
		);
		GLint world_cursor_x_offset_from_camera, world_cursor_y_offset_from_camera;
		util::camera::local_world_vector_to_world_vector
		(
			environment, 
			camera_local_world_cursor_x, camera_local_world_cursor_y, 
			&world_cursor_x_offset_from_camera, &world_cursor_y_offset_from_camera
		);
		environment.state.camera.xy.x = world_cursor_x - world_cursor_x_offset_from_camera;
		environment.state.camera.xy.y = world_cursor_y - world_cursor_y_offset_from_camera;
	}

	void tick(game_environment::Environment& environment)
	{
		glUseProgram(environment.state.rigid_body_velocity_integration_shader);
		glMemoryBarrier(GL_SHADER_STORAGE_BUFFER);
		glDispatchCompute
		(
			1l + environment.state.current_rigid_body_count / game_logic__util__rigid_body_VELOCITY_INTEGRATION_LOCAL_SIZE(environment),
			1l, 1l
		);

		GLint const fast_key_pressed{ glfwGetKey(environment.window, GLFW_KEY_LEFT_SHIFT) };
		GLint const slow_key_pressed{ glfwGetKey(environment.window, GLFW_KEY_LEFT_CONTROL) };
		GLfloat const distance
		{ 
			game_CAMERA_DEFAULT_DISTANCE_PER_TICK(environment) +
			fast_key_pressed * game_CAMERA_FAST_DISTANCE_PER_TICK_INCREASE(environment) - 
			slow_key_pressed * game_CAMERA_SLOW_DISTANCE_PER_TICK_DECREASE(environment) 
		};
		GLfloat const z_distance
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
		environment.state.camera.xy.x += x_displacement_x;
		environment.state.camera.xy.y += x_displacement_y;

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
		environment.state.camera.xy.x += y_displacement_x;
		environment.state.camera.xy.y += y_displacement_y;

		if (glfwGetKey(environment.window, GLFW_KEY_LEFT))
		{
			environment.state.camera.angle += angle;
		}
		if (glfwGetKey(environment.window, GLFW_KEY_RIGHT))
		{
			environment.state.camera.angle -= angle;
		}

		if (glfwGetKey(environment.window, GLFW_KEY_DOWN))
		{
			environment.state.camera.z += z_distance;
		}
		if (glfwGetKey(environment.window, GLFW_KEY_UP))
		{
			environment.state.camera.z -= z_distance;
		}

		// TODO: Separate into functions
		// TODO: Make sure to not loose precision due to large angles
		GLfloat const radians{ game_logic__util__spatial_TO_RADIANS(environment, environment.state.camera.angle) };
		GLfloat const right_x{ cos(radians) };
		GLfloat const right_y{ sin(radians) };
		environment.state.camera.view_rotation.column_0[0] = right_x;
		environment.state.camera.view_rotation.column_0[1] = -right_y;
		environment.state.camera.view_rotation.column_1[0] = right_y;
		environment.state.camera.view_rotation.column_1[1] = right_x;

		if (environment.state.point_grabbed)
		{
			GLint cursor_world_x, cursor_world_y;
			window_to_world::window_screen_cursor_position_to_world_position
			(
				environment, 
				&cursor_world_x, &cursor_world_y
			);
			environment.state.camera.xy.x += environment.state.grabbed_point.x - cursor_world_x;
			environment.state.camera.xy.y += environment.state.grabbed_point.y - cursor_world_y;
		}
	}

	void update_GPU_camera(game_environment::Environment& environment)
	{
		// TODO: Pass data directly to persistently mapped memory to avoid data copying
		// TODO: Store pointers instead of offsets to avoid additions
		std::memcpy
		(
			environment.state.camera_send_buffer + environment.state.camera_buffer_xy_offset, 
			&environment.state.camera.xy,
			sizeof(environment.state.camera.xy)
		);

		std::memcpy
		(
			environment.state.camera_send_buffer + environment.state.camera_buffer_angle_offset,
			&environment.state.camera.angle,
			sizeof(environment.state.camera.angle)
		);

		std::memcpy
		(
			environment.state.camera_send_buffer + environment.state.camera_buffer_z_offset,
			&environment.state.camera.z,
			sizeof(environment.state.camera.z)
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

		//glUseProgram(environment.state.shader);
		//glBindVertexArray(environment.state.vao);
		//glDrawArrays(GL_TRIANGLES, 0, 6);

		//glUseProgram(environment.state.rigid_body_debug_rendering_shader);
		glUseProgram(environment.state.triangle_draw_shader);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
		glDrawArrays(GL_TRIANGLES, 0, environment.state.current_triangle_count * 3);
	}

	void free(game_environment::Environment& environment)
	{
		::util::shader::delete_program(environment.state.shader);

		delete[] environment.state.camera_send_buffer;
		// TODO: Probably flip position and velocity buffers
		GLuint buffers[]
		{ 
			environment.state.camera_buffer, 
			environment.state.rigid_body_position_buffer, 
			environment.state.rigid_body_velocity_buffer, 
			environment.state.triangle_buffer, 
			environment.state.vertex_buffer
		};
		glDeleteBuffers(std::size(buffers), buffers);

		glDeleteVertexArrays(1, &environment.state.vao);
		glDeleteBuffers(1, &environment.state.vbo);

		glfwDestroyCursor(environment.state.grab_cursor);
	}
}
