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
#include "game_logic/util/rigid_body/TRIANGLE_BOUNDING_BOX_UPDATE_LOCAL_SIZE.h"
#include "game_logic/util/rigid_body/TRIANGLE_BOUNDING_BOX_PADDING.h"
#include "game_logic/util/rigid_body/Position.h"
#include "game_logic/util/rigid_body/Velocity.h"
#include "game_logic/util/rigid_body/Triangle.h"
#include "game_logic/util/rigid_body/Triangle_Bounding_Box.h"
#include "game_logic/util/proximity/initialize.h"
#include "game_logic/util/proximity/change_leaf_of_multinode_tree.h"
#include "game_logic/util/proximity/update_contacts.h"
#include "game_logic/util/proximity/insert_leaf_to_empty_tree.h"
#include "game_logic/util/proximity/insert_leaf_to_nonempty_tree.h"
#include "game_logic/util/proximity/print_tree.h"
#include "game_logic/util/proximity/is_empty.h"
#include "game_logic/util/proximity/has_single_node.h"
#include "game_logic/util/proximity/print_bounding_box.h"
#include "game_logic/util/proximity/compute_height.h"

#define game_logic_MAX_RIGID_BODY_COUNT(environment) \
	1000u * game_logic__util__rigid_body_VELOCITY_INTEGRATION_LOCAL_SIZE(environment)

#define game_logic_MAX_TRIANGLE_COUNT(environment) \
	1000u * game_logic__util__rigid_body_TRIANGLE_BOUNDING_BOX_UPDATE_LOCAL_SIZE(environment)

#define game_logic_MAX_VERTEX_COUNT(environment) \
	3u * game_logic_MAX_TRIANGLE_COUNT(environment)

#define game_logic_MAX_LEAF_COUNT(environment) \
	game_logic_MAX_TRIANGLE_COUNT(environment)

namespace game_logic
{
	void initialize(game_environment::Environment& environment)
	{
		environment.state.tick = 0u;

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
			util_shader_DEFINE("MAX_RIGID_BODY_COUNT", STRINGIFY(game_logic_MAX_RIGID_BODY_COUNT(environment))),
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
			util_shader_DEFINE("MAX_TRIANGLE_COUNT", STRINGIFY(game_logic_MAX_TRIANGLE_COUNT(environment))),
			util_shader_DEFINE("MAX_VERTEX_COUNT", STRINGIFY(game_logic_MAX_VERTEX_COUNT(environment))),
			util_shader_DEFINE("MAX_RIGID_BODY_COUNT", STRINGIFY(game_logic_MAX_RIGID_BODY_COUNT(environment))),
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
			vertex_shader,
			util_shader_VERSION,
			game_PROJECTION_SCALE_DEFINITION(environment),
			util_shader_DEFINE("MAX_TRIANGLE_COUNT", STRINGIFY(game_logic_MAX_TRIANGLE_COUNT(environment))),
			util_shader_DEFINE("MAX_VERTEX_COUNT", STRINGIFY(game_logic_MAX_VERTEX_COUNT(environment))),
			util_shader_DEFINE("BOUNDING_BOX_BINDING", STRINGIFY(game_logic__util_TRIANGLE_BOUNDING_BOX_BINDING)),
			util_shader_DEFINE("CAMERA_BINDING", STRINGIFY(game_CAMERA_BINDING)),
			::util::shader::file_to_string("util/triangle_bounding_box.vert")
		);
		::util::shader::set_shader_statically
		(
			fragment_shader,
			util_shader_VERSION,
			util_shader_DEFINE("COLOR", "vec4(1.0, 1.0, 1.0, 1.0)"),
			::util::shader::file_to_string("util/static_color.frag") // TODO: Should only be done once
		);
		environment.state.triangle_bounding_box_draw_shader = ::util::shader::create_program(vertex_shader, fragment_shader);

		::util::shader::set_shader_statically
		(
			vertex_shader,
			util_shader_VERSION,
			game_PROJECTION_SCALE_DEFINITION(environment),
			util_shader_DEFINE("CAMERA_BINDING", STRINGIFY(game_CAMERA_BINDING)),
			::util::shader::file_to_string("util/inner_bounding_box.vert")
		);
		::util::shader::set_shader_statically
		(
			fragment_shader,
			util_shader_VERSION,
			util_shader_DEFINE("COLOR", "vec4(1.0, 0.0, 0.0, 1.0)"),
			::util::shader::file_to_string("util/static_color.frag") // TODO: Should only be done once
		);
		environment.state.parent_bounding_box_draw_shader = ::util::shader::create_program(vertex_shader, fragment_shader);
		environment.state.parent_bounding_box_uniform_location = glGetUniformLocation
		(
			environment.state.parent_bounding_box_draw_shader, 
			"bounding_box"
		);

		::util::shader::delete_shader(vertex_shader);
		::util::shader::delete_shader(fragment_shader);

		GLuint const compute_shader{ ::util::shader::create_shader(GL_COMPUTE_SHADER) };

		::util::shader::set_shader_statically
		(
			compute_shader,
			util_shader_VERSION, 
			util_shader_DEFINE("MAX_RIGID_BODY_COUNT", STRINGIFY(game_logic_MAX_RIGID_BODY_COUNT(environment))),
			util_shader_DEFINE("POSITION_BINDING", STRINGIFY(game_logic__util_RIGID_BODY_POSITION_BINDING)),
			util_shader_DEFINE("VELOCITY_BINDING", STRINGIFY(game_logic__util_RIGID_BODY_VELOCITY_BINDING)),
			util_shader_DEFINE("LOCAL_SIZE", STRINGIFY(game_logic__util__rigid_body_VELOCITY_INTEGRATION_LOCAL_SIZE(environment))),
			::util::shader::file_to_string("util/rigid_body_velocity_integration.comp")
		);
		environment.state.rigid_body_velocity_integration_shader = ::util::shader::create_program(compute_shader);

		// TODO: Think about how to deal with this temporary variable
		std::string padding_definition
		{ 
			"#define PADDING " + std::to_string(game_logic__util__rigid_body_TRIANGLE_BOUNDING_BOX_PADDING(environment)) + '\n'
		};
		::util::shader::set_shader_statically
		(
			compute_shader,
			util_shader_VERSION,
			padding_definition,
			util_shader_DEFINE("MAX_TRIANGLE_COUNT", STRINGIFY(game_logic_MAX_TRIANGLE_COUNT(environment))),
			util_shader_DEFINE("MAX_VERTEX_COUNT", STRINGIFY(game_logic_MAX_VERTEX_COUNT(environment))),
			util_shader_DEFINE("MAX_RIGID_BODY_COUNT", STRINGIFY(game_logic_MAX_RIGID_BODY_COUNT(environment))),
			util_shader_DEFINE("POSITION_BINDING", STRINGIFY(game_logic__util_RIGID_BODY_POSITION_BINDING)),
			util_shader_DEFINE("TRIANGLE_BINDING", STRINGIFY(game_logic__util_TRIANGLE_BINDING)),
			util_shader_DEFINE("VERTEX_BINDING", STRINGIFY(game_logic__util_VERTEX_BINDING)),
			util_shader_DEFINE("BOUNDING_BOX_BINDING", STRINGIFY(game_logic__util_TRIANGLE_BOUNDING_BOX_BINDING)),
			util_shader_DEFINE("CHANGED_BOUNDING_BOX_BINDING", STRINGIFY(game_logic__util_TRIANGLE_CHANGED_BOUNDING_BOX_BINDING)),
			util_shader_DEFINE("VELOCITY_BINDING", STRINGIFY(game_logic__util_RIGID_BODY_VELOCITY_BINDING)),
			util_shader_DEFINE("LOCAL_SIZE", STRINGIFY(game_logic__util__rigid_body_TRIANGLE_BOUNDING_BOX_UPDATE_LOCAL_SIZE(environment))),
			util_shader_DEFINE("RADIAN_INVERSE", STRINGIFY(game_logic__util__spatial_RADIAN_INVERSE(environment))),
			::util::shader::file_to_string("util/triangle_bounding_box_update.comp")
		);
		environment.state.triangle_bounding_box_update_shader = ::util::shader::create_program(compute_shader);

		::util::shader::delete_shader(compute_shader);

		// TODO: Consider putting buffers next to each other in game state
		// TODO: Probably flip position of position and velocity buffer
		GLuint buffers[]
		{ 
			environment.state.camera_buffer, 
			environment.state.rigid_body_position_buffer, 
			environment.state.rigid_body_velocity_buffer, 
			environment.state.triangle_buffer, 
			environment.state.vertex_buffer, 
			environment.state.bounding_box_buffer, 
			environment.state.changed_bounding_box_buffer
		};
		glCreateBuffers(std::size(buffers), buffers);
		environment.state.camera_buffer = buffers[0];
		environment.state.rigid_body_position_buffer = buffers[1];
		environment.state.rigid_body_velocity_buffer = buffers[2];
		environment.state.triangle_buffer = buffers[3];
		environment.state.vertex_buffer = buffers[4];
		environment.state.bounding_box_buffer = buffers[5];
		environment.state.changed_bounding_box_buffer = buffers[6];

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

		environment.state.current_rigid_body_count = 1u * game_logic__util__rigid_body_TRIANGLE_BOUNDING_BOX_UPDATE_LOCAL_SIZE(environment);//500000u;
		environment.state.current_triangle_count = 1u * environment.state.current_rigid_body_count;

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

			GLuint const block_index
			{
				glGetProgramResourceIndex(environment.state.rigid_body_velocity_integration_shader, GL_SHADER_STORAGE_BLOCK, "Positions")
			};
			GLenum const buffer_size_label{ GL_BUFFER_DATA_SIZE };
			glGetProgramResourceiv
			(
				environment.state.rigid_body_velocity_integration_shader, GL_SHADER_STORAGE_BLOCK, block_index,
				1, &buffer_size_label, 1, nullptr, &environment.state.rigid_body_position_buffer_size
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
					game_logic__util__spatial_FROM_RADIANS(environment, i * 0.1f), 
					0
				};
				if (i < 2)
				{
					position.position.x -= game_logic__util__spatial_FROM_METERS(environment, -20.0f);
				}
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
				0u
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

			GLuint const block_index
			{
				glGetProgramResourceIndex(environment.state.rigid_body_velocity_integration_shader, GL_SHADER_STORAGE_BLOCK, "Velocities")
			};
			GLenum const buffer_size_label{ GL_BUFFER_DATA_SIZE };
			glGetProgramResourceiv
			(
				environment.state.rigid_body_velocity_integration_shader, GL_SHADER_STORAGE_BLOCK, block_index,
				1, &buffer_size_label, 1, nullptr, &environment.state.rigid_body_velocity_buffer_size
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
					game_RADIANS_PER_SECOND_TO_ANGLE_PER_TICK(environment, 0.5f), 
					0
				};
				/*if (2 <= i && i < environment.state.current_rigid_body_count - 2)
				{
					velocity.velocity.x = 0;
					velocity.velocity.y = 0;
					velocity.angle_velocity = 0;
				}*/
				std::memcpy
				(
					initial_velocities + environment.state.rigid_body_velocity_buffer_v_offset + i * environment.state.rigid_body_velocity_buffer_v_stride, 
					&velocity, sizeof(velocity)
				);
			}

			glNamedBufferStorage
			(
				environment.state.rigid_body_velocity_buffer, environment.state.rigid_body_velocity_buffer_size, initial_velocities,
				0u
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

			GLuint const block_index
			{
				glGetProgramResourceIndex(environment.state.triangle_draw_shader, GL_SHADER_STORAGE_BLOCK, "Triangles")
			};
			GLenum const buffer_size_label{ GL_BUFFER_DATA_SIZE };
			glGetProgramResourceiv
			(
				environment.state.triangle_draw_shader, GL_SHADER_STORAGE_BLOCK, block_index,
				1, &buffer_size_label, 1, nullptr, &environment.state.triangle_buffer_size
			);

			// TODO: Don't initialize a few triangles by copying over the ENTIRE buffer 
			// content from CPU to GPU like this. Instead, use persistent mapping 
			// for both initialization and updating.
			unsigned char* const initial_triangles = new unsigned char[environment.state.triangle_buffer_size];

			for (GLuint i = 0; i < environment.state.current_triangle_count; ++i)
			{
				/*util::rigid_body::Triangle triangle
				{
					{(2u * i) % 4u, (2u * i + 1u) % 4u, (2u * i + 2u) % 4u}, i / 2u
				};*/
				util::rigid_body::Triangle triangle
				{
					{(2u * i) % 4u, (2u * i + 1u) % 4u, (2u * i + 2u) % 4u}, i
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
				0u
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

			GLuint const block_index
			{
				glGetProgramResourceIndex(environment.state.triangle_draw_shader, GL_SHADER_STORAGE_BLOCK, "Vertices")
			};
			GLenum const buffer_size_label{ GL_BUFFER_DATA_SIZE };
			glGetProgramResourceiv
			(
				environment.state.triangle_draw_shader, GL_SHADER_STORAGE_BLOCK, block_index,
				1, &buffer_size_label, 1, nullptr, &environment.state.vertex_buffer_size
			);
			// TODO: Don't initialize a few vertices by copying over the ENTIRE buffer 
			// content from CPU to GPU like this. Instead, use persistent mapping 
			// for both initialization and updating.
			unsigned char* const initial_vertices = new unsigned char[environment.state.vertex_buffer_size];
			GLfloat vertex[2];
			GLfloat const r{ 0.5f };
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
				0u
			);

			delete[] initial_vertices;

			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, game_logic__util_VERTEX_BINDING, environment.state.vertex_buffer);
		}

		 { // Bounding box buffer
			GLuint const boxes_index
			{
				glGetProgramResourceIndex(environment.state.triangle_bounding_box_update_shader, GL_BUFFER_VARIABLE, "Bounding_Boxes.boxes")
			};

			GLenum const prop_labels[]{ GL_OFFSET, GL_ARRAY_STRIDE };
			GLint props[std::size(prop_labels)];
			glGetProgramResourceiv
			(
				environment.state.triangle_bounding_box_update_shader, GL_BUFFER_VARIABLE, boxes_index,
				std::size(prop_labels), prop_labels, 2, nullptr, props
			);
			// TODO: Consider putting offset and stride contigously in game state
			environment.state.bounding_box_buffer_boxes_offset = props[0];
			environment.state.bounding_box_buffer_boxes_stride = props[1];

			GLuint const block_index
			{
				glGetProgramResourceIndex(environment.state.triangle_bounding_box_update_shader, GL_SHADER_STORAGE_BLOCK, "Bounding_Boxes")
			};
			GLenum const buffer_size_label{ GL_BUFFER_DATA_SIZE };
			glGetProgramResourceiv
			(
				environment.state.triangle_bounding_box_update_shader, GL_SHADER_STORAGE_BLOCK, block_index,
				1, &buffer_size_label, 1, nullptr, &environment.state.bounding_box_buffer_size
			);

			// TODO: Don't initialize a few boxes by copying over the ENTIRE buffer 
			// content from CPU to GPU like this. Instead, use persistent mapping 
			// for both initialization and updating.
			unsigned char* const initial_boxes = new unsigned char[environment.state.bounding_box_buffer_size];

			util::rigid_body::Triangle_Bounding_Box box
			{ 
				{ 
					0, 
					0 
				},
				{ 
					-1, 
					-1
				} 
			};
			for (GLuint i = 0; i < environment.state.current_triangle_count; ++i)
			{
				std::memcpy
				(
					initial_boxes + environment.state.bounding_box_buffer_boxes_offset + i * environment.state.bounding_box_buffer_boxes_stride,
					&box, sizeof(box)
				);
			}

			glNamedBufferStorage
			(
				environment.state.bounding_box_buffer, environment.state.bounding_box_buffer_size, initial_boxes, 
				0u
			);

			delete[] initial_boxes;

			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, game_logic__util_TRIANGLE_BOUNDING_BOX_BINDING, environment.state.bounding_box_buffer);
		}

		 { // Changed bounding box buffer
			 {
				 GLuint const size_index
				 {
					 glGetProgramResourceIndex(environment.state.triangle_bounding_box_update_shader, GL_BUFFER_VARIABLE, "Changed_Bounding_Boxes.size")
				 };
				 GLenum const offset_label{ GL_OFFSET };
				 glGetProgramResourceiv
				 (
					 environment.state.triangle_bounding_box_update_shader, GL_BUFFER_VARIABLE, size_index,
					 1, &offset_label, 1, nullptr, &environment.state.changed_bounding_box_buffer_size_offset
				 );
			 }

			 {
				 GLuint const boxes_index_index
				 {
					 glGetProgramResourceIndex(environment.state.triangle_bounding_box_update_shader, GL_BUFFER_VARIABLE, "Changed_Bounding_Boxes.boxes[0].index")
				 };
				 GLenum const prop_labels[]{ GL_OFFSET, GL_TOP_LEVEL_ARRAY_STRIDE };
				 GLint props[std::size(prop_labels)];
				 glGetProgramResourceiv
				 (
					 environment.state.triangle_bounding_box_update_shader, GL_BUFFER_VARIABLE, boxes_index_index,
					 std::size(prop_labels), prop_labels, 2, nullptr, props
				 );
				 // TODO: Consider putting offset and stride contigously in game state
				 environment.state.changed_bounding_box_buffer_boxes_index_offset = props[0];
				 environment.state.changed_bounding_box_buffer_boxes_stride = props[1];

				 GLenum const offset_label{ GL_OFFSET };

				 GLuint const boxes_min_x_index
				 {
					 glGetProgramResourceIndex(environment.state.triangle_bounding_box_update_shader, GL_BUFFER_VARIABLE, "Changed_Bounding_Boxes.boxes[0].min_x")
				 };
				 glGetProgramResourceiv
				 (
					 environment.state.triangle_bounding_box_update_shader, GL_BUFFER_VARIABLE, boxes_min_x_index,
					 1, &offset_label, 1, nullptr, &environment.state.changed_bounding_box_buffer_boxes_min_x_offset
				 );

				 GLuint const boxes_min_y_index
				 {
					 glGetProgramResourceIndex(environment.state.triangle_bounding_box_update_shader, GL_BUFFER_VARIABLE, "Changed_Bounding_Boxes.boxes[0].min_y")
				 };
				 glGetProgramResourceiv
				 (
					 environment.state.triangle_bounding_box_update_shader, GL_BUFFER_VARIABLE, boxes_min_y_index,
					 1, &offset_label, 1, nullptr, &environment.state.changed_bounding_box_buffer_boxes_min_y_offset
				 );

				 GLuint const boxes_max_x_index
				 {
					 glGetProgramResourceIndex(environment.state.triangle_bounding_box_update_shader, GL_BUFFER_VARIABLE, "Changed_Bounding_Boxes.boxes[0].max_x")
				 };
				 glGetProgramResourceiv
				 (
					 environment.state.triangle_bounding_box_update_shader, GL_BUFFER_VARIABLE, boxes_max_x_index,
					 1, &offset_label, 1, nullptr, &environment.state.changed_bounding_box_buffer_boxes_max_x_offset
				 );

				 GLuint const boxes_max_y_index
				 {
					 glGetProgramResourceIndex(environment.state.triangle_bounding_box_update_shader, GL_BUFFER_VARIABLE, "Changed_Bounding_Boxes.boxes[0].max_y")
				 };
				 glGetProgramResourceiv
				 (
					 environment.state.triangle_bounding_box_update_shader, GL_BUFFER_VARIABLE, boxes_max_y_index,
					 1, &offset_label, 1, nullptr, &environment.state.changed_bounding_box_buffer_boxes_max_y_offset
				 );
			}
			 
			 GLuint const block_index
			 {
				 glGetProgramResourceIndex(environment.state.triangle_bounding_box_update_shader, GL_SHADER_STORAGE_BLOCK, "Changed_Bounding_Boxes")
			 };
			 GLenum const buffer_size_label{ GL_BUFFER_DATA_SIZE };
			 glGetProgramResourceiv
			 (
				 environment.state.triangle_bounding_box_update_shader, GL_SHADER_STORAGE_BLOCK, block_index,
				 1, &buffer_size_label, 1, nullptr, &environment.state.changed_bounding_box_buffer_size
			 );

			 glNamedBufferStorage
			 (
				 environment.state.changed_bounding_box_buffer, environment.state.changed_bounding_box_buffer_size, nullptr,
				 GL_MAP_READ_BIT | GL_MAP_PERSISTENT_BIT
			 );
			 glClearNamedBufferSubData
			 (
				 environment.state.changed_bounding_box_buffer, 
				 GL_R32UI, 
				 environment.state.changed_bounding_box_buffer_size_offset, sizeof(GLuint), 
				 GL_RED, GL_UNSIGNED_INT, 
				 nullptr
			 );

			 glBindBufferBase(GL_SHADER_STORAGE_BUFFER, game_logic__util_TRIANGLE_CHANGED_BOUNDING_BOX_BINDING, environment.state.changed_bounding_box_buffer);

			 environment.state.changed_bounding_boxes_mapping = static_cast<unsigned char*>
			 (
				glMapNamedBufferRange
				(
					environment.state.changed_bounding_box_buffer, 
					0u, environment.state.changed_bounding_box_buffer_size, 
					GL_MAP_READ_BIT | GL_MAP_PERSISTENT_BIT
				)
			 );
		 }

		util::proximity::initialize
		(
			environment.state.proximity_tree, game_logic_MAX_LEAF_COUNT(environment)
		);
		util::proximity::insert_leaf_to_empty_tree(
			environment.state.proximity_tree, game_logic_MAX_LEAF_COUNT(environment),
			0, 
			0, 0, -1, -1
		);
		for (GLuint i = 1; i < environment.state.current_triangle_count; ++i)
		{
			util::proximity::insert_leaf_to_nonempty_tree(
				environment.state.proximity_tree, game_logic_MAX_LEAF_COUNT(environment),
				i, 
				0, 0, -1, -1
			);
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

		std::cout << "Vertex buffer (" << environment.state.vertex_buffer << "):" << std::endl;
		std::cout << "size: " << environment.state.vertex_buffer_size << std::endl;
		std::cout << "vertices offset: " << environment.state.vertex_buffer_vertices_offset << std::endl;
		std::cout << "vertices stride: " << environment.state.vertex_buffer_vertices_stride << std::endl;
		std::cout << std::endl;

		std::cout << "Bounding box buffer (" << environment.state.bounding_box_buffer << "):" << std::endl;
		std::cout << "size: " << environment.state.bounding_box_buffer_size << std::endl;
		std::cout << "boxes offset: " << environment.state.bounding_box_buffer_boxes_offset << std::endl;
		std::cout << "boxes stride: " << environment.state.bounding_box_buffer_boxes_stride << std::endl;
		std::cout << std::endl;

		std::cout << "Changed bounding box buffer (" << environment.state.changed_bounding_box_buffer << "):" << std::endl;
		std::cout << "size: " << environment.state.changed_bounding_box_buffer_size << std::endl;
		std::cout << "push index offset: " << environment.state.changed_bounding_box_buffer_size_offset << std::endl;
		std::cout << "boxes stride: " << environment.state.changed_bounding_box_buffer_boxes_stride << std::endl;
		std::cout << "boxes index offset: " << environment.state.changed_bounding_box_buffer_boxes_index_offset << std::endl;
		std::cout << "boxes min_x offset: " << environment.state.changed_bounding_box_buffer_boxes_min_x_offset << std::endl;
		std::cout << "boxes min_y offset: " << environment.state.changed_bounding_box_buffer_boxes_min_y_offset << std::endl;
		std::cout << "boxes max_x offset: " << environment.state.changed_bounding_box_buffer_boxes_max_x_offset << std::endl;
		std::cout << "boxes max_y offset: " << environment.state.changed_bounding_box_buffer_boxes_max_y_offset << std::endl;
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

	// TODO: Move to ::util::math
	inline GLuint ceil_div(GLuint numerator, GLuint denominator)
	{
		return numerator / denominator + (numerator % denominator != 0);
	}

	void tick(game_environment::Environment& environment)
	{
		// TODO: See if we can load and run these opengl commands in GPU buffer (using indirect something)

		glUseProgram(environment.state.rigid_body_velocity_integration_shader);
		glMemoryBarrier(GL_SHADER_STORAGE_BUFFER);
		glDispatchCompute
		(
			ceil_div(environment.state.current_rigid_body_count, game_logic__util__rigid_body_VELOCITY_INTEGRATION_LOCAL_SIZE(environment)), 
			1u, 1u
		);

		glUseProgram(environment.state.triangle_bounding_box_update_shader);
		glMemoryBarrier(GL_SHADER_STORAGE_BUFFER);
		glDispatchCompute
		(
			ceil_div(environment.state.current_triangle_count, game_logic__util__rigid_body_TRIANGLE_BOUNDING_BOX_UPDATE_LOCAL_SIZE(environment)), 
			1u, 1u
		);
		glMemoryBarrier(GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT);
		GLsync const fence{ glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0u) };
		glFlush();

		// TODO: Other operations that we let the GPU perform while it is working on 
		// updating and returning the bounding boxes to the CPU. Example: Removing 
		// contacts whose bounding-box-pairs no longer overlap.

		// TODO: Potential one-time CPU operations we must do before 
		// waiting for the GPU to be done.

		GLenum fence_status = glClientWaitSync(fence, 0u, 0u);
		while (fence_status != GL_ALREADY_SIGNALED && fence_status != GL_CONDITION_SATISFIED)
		{
			// TODO: Do something useful but not necessary while we wait. 
			// Example: Optimize proximity tree.
			fence_status = glClientWaitSync(fence, 0u, 0u);
		}
		
		/*switch (fence_status)
		{
			case GL_ALREADY_SIGNALED:
				std::cout << "Already signaled" << std::endl;
				break;
			case GL_CONDITION_SATISFIED:
				std::cout << "Condition satisfied" << std::endl;
				break;
		}*/

		std::memcpy
		(
			&environment.state.proximity_tree.changed_leaf_count,
			environment.state.changed_bounding_boxes_mapping + environment.state.changed_bounding_box_buffer_size_offset, 
			sizeof(GLuint)
		);
		// TODO: Fix shader invocations working on out of bounds data
		// (by skipping over them when iterating here?)
		/*if (size > 10u)
		{
			std::cout << size << std::endl;
		}*/
		glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT); // TODO: Make sure this is correct
		glClearNamedBufferSubData
		(
			environment.state.changed_bounding_box_buffer,
			GL_R32UI,
			environment.state.changed_bounding_box_buffer_size_offset, sizeof(GLuint),
			GL_RED, GL_UNSIGNED_INT,
			nullptr
		);

		if (util::proximity::is_empty(environment.state.proximity_tree))
		{

		}
		else if (util::proximity::has_single_node(environment.state.proximity_tree, game_logic_MAX_LEAF_COUNT(environment)))
		{

		}
		else
		{
			unsigned char const* index_start{ environment.state.changed_bounding_boxes_mapping + environment.state.changed_bounding_box_buffer_boxes_index_offset };
			unsigned char const* min_x_start{ environment.state.changed_bounding_boxes_mapping + environment.state.changed_bounding_box_buffer_boxes_min_x_offset };
			unsigned char const* min_y_start{ environment.state.changed_bounding_boxes_mapping + environment.state.changed_bounding_box_buffer_boxes_min_y_offset };
			unsigned char const* max_x_start{ environment.state.changed_bounding_boxes_mapping + environment.state.changed_bounding_box_buffer_boxes_max_x_offset };
			unsigned char const* max_y_start{ environment.state.changed_bounding_boxes_mapping + environment.state.changed_bounding_box_buffer_boxes_max_y_offset };

			for (GLuint i{ 0 }; i < environment.state.proximity_tree.changed_leaf_count; ++i)
			{
				GLuint index;
				GLint min_x, min_y, max_x, max_y;
				std::memcpy(&index, index_start, sizeof(GLuint));
				std::memcpy(&min_x, min_x_start, sizeof(GLint));
				std::memcpy(&min_y, min_y_start, sizeof(GLint));
				std::memcpy(&max_x, max_x_start, sizeof(GLint));
				std::memcpy(&max_y, max_y_start, sizeof(GLint));

				//std::cout << index << ": ";
				//util::proximity::print(std::cout, game_state::proximity::Bounding_Box{ min_x, min_y, max_x, max_y }) << std::endl;

				util::proximity::change_leaf_of_multinode_tree
				(
					environment.state.proximity_tree, game_logic_MAX_LEAF_COUNT(environment), 
					index, i, min_x, min_y, max_x, max_y
				);

				index_start += environment.state.changed_bounding_box_buffer_boxes_stride;
				min_x_start += environment.state.changed_bounding_box_buffer_boxes_stride;
				min_y_start += environment.state.changed_bounding_box_buffer_boxes_stride;
				max_x_start += environment.state.changed_bounding_box_buffer_boxes_stride;
				max_y_start += environment.state.changed_bounding_box_buffer_boxes_stride;

				//std::cout << i << ": " << index << std::endl;
			}
		}
		if (environment.state.tick % 120u == 0u)
		{
			std::cout << "Height: " << util::proximity::compute_height
			(
				environment.state.proximity_tree, game_logic_MAX_LEAF_COUNT(environment)
			) << std::endl;
			std::cout << "Changed leaf count: " << environment.state.proximity_tree.changed_leaf_count << std::endl;
		}

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

		++environment.state.tick;
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

	void draw_inner_bounding_boxes
	(
		game_environment::Environment const& environment, 
		GLuint node_index
	)
	{
		if (node_index < game_logic_MAX_LEAF_COUNT(environment))
		{
			return;
		}
		game_state::proximity::Node const& node
		{
			environment.state.proximity_tree.nodes[node_index]
		};
		draw_inner_bounding_boxes(environment, node.children[0]);
		draw_inner_bounding_boxes(environment, node.children[1]);
		glUniform4i
		(
			environment.state.parent_bounding_box_uniform_location,
			node.bounding_box.min.x, node.bounding_box.min.y, 
			node.bounding_box.max.x, node.bounding_box.max.y
		);
		glDrawArrays(GL_LINES, 0, 8);
	}

	// TODO: Rename to draw to not confuse with arbitrary OpenGL rendering commands 
	// which includes compute shaders
	void render(game_environment::Environment& environment)
	{
		glClear(GL_COLOR_BUFFER_BIT);

		update_GPU_camera(environment);

		// TODO: Indirect drawing
		
		//glUseProgram(environment.state.shader);
		//glBindVertexArray(environment.state.vao);
		//glDrawArrays(GL_TRIANGLES, 0, 6);

		//glUseProgram(environment.state.rigid_body_debug_rendering_shader);
		glUseProgram(environment.state.triangle_draw_shader);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
		glDrawArrays(GL_TRIANGLES, 0, environment.state.current_triangle_count * 3);

		//glUseProgram(environment.state.triangle_bounding_box_draw_shader);
		//glDrawArrays(GL_LINES, 0, environment.state.current_triangle_count * 8);

		/*if (!util::proximity::is_empty(environment.state.proximity_tree))
		{
			glUseProgram(environment.state.parent_bounding_box_draw_shader);
			draw_inner_bounding_boxes(environment, environment.state.proximity_tree.root);
		}*/
	}

	void free(game_environment::Environment& environment)
	{
		glUnmapNamedBuffer(environment.state.changed_bounding_box_buffer);

		::util::shader::delete_program(environment.state.shader);

		delete[] environment.state.camera_send_buffer;
		// TODO: Probably flip position and velocity buffers
		GLuint buffers[]
		{ 
			environment.state.camera_buffer, 
			environment.state.rigid_body_position_buffer, 
			environment.state.rigid_body_velocity_buffer, 
			environment.state.triangle_buffer, 
			environment.state.vertex_buffer, 
			environment.state.bounding_box_buffer, 
			environment.state.changed_bounding_box_buffer
		};
		glDeleteBuffers(std::size(buffers), buffers);

		glDeleteVertexArrays(1, &environment.state.vao);
		glDeleteBuffers(1, &environment.state.vbo);

		glfwDestroyCursor(environment.state.grab_cursor);
	}
}
