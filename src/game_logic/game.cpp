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
#include "game_logic/util/spatial/FROM_RADIANS.h"
#include "game_logic/util/spatial/TO_METERS.h"
#include "game_logic/util/camera/unit_z_vector_to_camera_local_world_vector.h"
#include "game_logic/util/camera/increase_camera_angle.h"
#include "game_logic/util/camera/local_world_vector_to_world_vector.h"
#include "game_logic/util/camera/local_world_position_to_world_position.h"
#include "game_logic/util/rigid_body/VELOCITY_INTEGRATION_LOCAL_SIZE.h"
#include "game_logic/util/rigid_body/TRIANGLE_BOUNDING_BOX_UPDATE_LOCAL_SIZE.h"
#include "game_logic/util/rigid_body/OLD_TRIANGLE_CONTACT_UPDATE_LOCAL_SIZE.h"
#include "game_logic/util/rigid_body/NEW_TRIANGLE_CONTACT_LOCAL_SIZE.h"
#include "game_logic/util/rigid_body/WARM_START_CONTACT_IMPULSES_LOCAL_SIZE.h"
#include "game_logic/util/rigid_body/SOLVE_CONTACT_VELOCITIES_LOCAL_SIZE.h"
#include "game_logic/util/rigid_body/TRIANGLE_BOUNDING_BOX_PADDING.h"
#include "game_logic/util/rigid_body/Position.h"
#include "game_logic/util/rigid_body/Velocity.h"
#include "game_state/rigid_body/Triangle.h"
#include "game_logic/util/rigid_body/Triangle_Bounding_Box.h"
#include "game_logic/util/proximity/initialize.h"
#include "game_logic/util/proximity/change_leaf_of_multinode_tree.h"
#include "game_logic/util/proximity/remove_contacts.h"
#include "game_logic/util/proximity/add_contacts.h"
#include "game_logic/util/proximity/insert_leaf_to_empty_tree.h"
#include "game_logic/util/proximity/insert_leaf_to_nonempty_tree.h"
#include "game_logic/util/proximity/move_contact.h"
#include "game_logic/util/proximity/print_tree.h"
#include "game_logic/util/proximity/print_leaf_contacts.h"
#include "game_logic/util/proximity/is_empty.h"
#include "game_logic/util/proximity/has_single_node.h"
#include "game_logic/util/proximity/print_bounding_box.h"
#include "game_logic/util/proximity/compute_height.h"
#include "game_logic/util/proximity/update_contacts.h"
#include "game_logic/util/proximity/query_point_of_multinode_tree.h"
#include <algorithm>
#include <chrono>
#include "game_logic/cursor_types/include.h"

#define M_PI 3.14159265358979323846f

#define GRAVITY_SAMPLE_STEP(environment) 0.1f
#define GRAVITY_SOURCE_GRAB_RADIUS(environment) 0.5f * game_logic__util__spatial_METER(environment)
#define GRAVITY_SOURCE_LIGHT_DISTANCE(environment) 1.0f * game_logic__util__spatial_METER(environment)

#define INTEGRATE_FLUID_VELOCITY_LOCAL_SIZE(environment) \
	game_logic__util__rigid_body_DEFAULT_COMPUTE_SHADER_LOCAL_SIZE(environment)

#define PERSIST_FLUID_CONTACT_LOCAL_SIZE(environment) \
	game_logic__util__rigid_body_DEFAULT_COMPUTE_SHADER_LOCAL_SIZE(environment)

#define NEW_FLUID_CONTACT_LOCAL_SIZE(environment) \
	game_logic__util__rigid_body_DEFAULT_COMPUTE_SHADER_LOCAL_SIZE(environment)

#define WARM_START_FLUID_CONTACTS_LOCAL_SIZE(environment) \
	game_logic__util__rigid_body_DEFAULT_COMPUTE_SHADER_LOCAL_SIZE(environment)

#define SOLVE_FLUID_CONTACTS_LOCAL_SIZE(environment) \
	game_logic__util__rigid_body_DEFAULT_COMPUTE_SHADER_LOCAL_SIZE(environment)

#define SOLVE_FLUID_CONTACTS_IMPULSE_SCALE(environment) 0.2f

#define FLUID_INVERSE_MASS(environment) 40.0f
#define FLUID_STRENGTH_RADIUS(environment) 0.1f * game_logic__util__spatial_METER(environment);//0.21f * game_logic__util__spatial_METER(environment)//0.35f * game_logic__util__spatial_METER(environment)
#define FLUID_MAX_STRENGTH(environment) 0.1f * game_logic__util__spatial_METER(environment)//0.02f * game_logic__util__spatial_METER(environment)
#define FLUID_TARGET_RADIUS(environment) 0.2f * game_logic__util__spatial_METER(environment)
#define FLUID_TARGET_VELOCITY_SCALE(environment) 0.03f

#define game_logic_MAX_FLUID_PARTICLE_COUNT(environment) \
	100u * INTEGRATE_FLUID_VELOCITY_LOCAL_SIZE(environment)

#define game_logic_FLUID_PARTICLE_PHYSICAL_RADIUS(environment) \
	static_cast<GLint>(0.2f * game_logic__util__spatial_METER(environment))

#define game_logic_FLUID_PARTICLE_BOUNDING_BOX_PADDING(environment) \
	game_logic__util__spatial_FROM_METERS(environment, 0.01f)//0.1f)//0.2f)//0.1f)

#define game_logic_FLUID_PARTICLE_DRAW_RADIUS(environment) \
	game_logic__util__spatial_FLOAT_FROM_METERS(environment, 1.0f)

#define PERSIST_FLUID_TRIANGLE_CONTACT_LOCAL_SIZE(environment) \
	game_logic__util__rigid_body_DEFAULT_COMPUTE_SHADER_LOCAL_SIZE(environment)

#define NEW_FLUID_TRIANGLE_CONTACT_LOCAL_SIZE(environment) \
	game_logic__util__rigid_body_DEFAULT_COMPUTE_SHADER_LOCAL_SIZE(environment)

#define WARM_START_FLUID_TRIANGLE_CONTACTS_LOCAL_SIZE(environment) \
	game_logic__util__rigid_body_DEFAULT_COMPUTE_SHADER_LOCAL_SIZE(environment)

#define SOLVE_FLUID_TRIANGLE_CONTACTS_LOCAL_SIZE(environment) \
	game_logic__util__rigid_body_DEFAULT_COMPUTE_SHADER_LOCAL_SIZE(environment)

#define SOLVE_FLUID_TRIANGLE_CONTACTS_IMPULSE_SCALE(environment) 0.2f

#define FLUID_TRIANGLE_RADIUS(environment) 0.2f * game_logic__util__spatial_METER(environment)

#define game_logic_MAX_RIGID_BODY_COUNT(environment) \
	100u * game_logic__util__rigid_body_VELOCITY_INTEGRATION_LOCAL_SIZE(environment)

#define game_logic_TRIANGLE_LEAFS_BASE_INDEX(environment) \
	game_logic_MAX_FLUID_PARTICLE_COUNT(environment)

#define game_logic_MAX_TRIANGLE_COUNT(environment) \
	100u * game_logic__util__rigid_body_TRIANGLE_BOUNDING_BOX_UPDATE_LOCAL_SIZE(environment)

#define game_logic_MAX_VERTEX_COUNT(environment) \
	3u * game_logic_MAX_TRIANGLE_COUNT(environment)

#define game_logic_MAX_LEAF_COUNT(environment) \
	game_logic_MAX_FLUID_PARTICLE_COUNT(environment) + game_logic_MAX_TRIANGLE_COUNT(environment)

#define MAX_FLUID_CONTACT_COUNT(environment) \
	(2000u * game_logic_MAX_FLUID_PARTICLE_COUNT(environment))

#define MAX_FLUID_TRIANGLE_CONTACT_COUNT(environment) \
	(1000u * game_logic_MAX_FLUID_PARTICLE_COUNT(environment))

#define game_logic_MAX_TRIANGLE_CONTACT_COUNT(environment) \
	(2000u * game_logic_MAX_TRIANGLE_COUNT(environment))

#define FLUID_TRIANGLE_CONTACT_BASE_INDEX(environment) \
	MAX_FLUID_CONTACT_COUNT(environment)

#define TRIANGLE_CONTACT_BASE_INDEX(environment) \
	(FLUID_TRIANGLE_CONTACT_BASE_INDEX(environment) + MAX_FLUID_TRIANGLE_CONTACT_COUNT(environment))

#define MAX_CONTACT_COUNT(environment) \
	(TRIANGLE_CONTACT_BASE_INDEX(environment) + game_logic_MAX_TRIANGLE_CONTACT_COUNT(environment))

#define game_logic_TANGENT_IMPULSE_SCALE(environment) 0.05f
#define game_logic_NORMAL_IMPULSE_SCALE(environment) 0.05f
#define game_logic_DIRECT_POSITION_CORRECTION(environment) 0
#define game_logic_ALLOWED_PENETRATION(environment) game_logic__util__spatial_FLOAT_FROM_METERS(environment, 0.05f)
#define game_logic_PENETRATION_VELOCITY_SCALE(environment) 0.002f * game_logic__util__spatial_METER_INVERSE(environment)
#define game_logic_POSITION_IMPULSE_SCALE(environment) 0.1f * game_logic__util__spatial_METER_INVERSE(environment)

#define game_logic_MAX_DISTANCE_CONSTRAINT_COUNT(environment) \
	1000000u

#define game_logic_UPDATE_DISTANCE_CONSTRAINTS_LOCAL_SIZE(environmnent) \
	game_logic__util__rigid_body_DEFAULT_COMPUTE_SHADER_LOCAL_SIZE(environment)

#define game_logic_WARM_START_DISTANCE_CONSTRAINTS_LOCAL_SIZE(environmnent) \
	game_logic__util__rigid_body_DEFAULT_COMPUTE_SHADER_LOCAL_SIZE(environment)

#define game_logic_SOLVE_DISTANCE_CONSTRAINTS_LOCAL_SIZE(environmnent) \
	game_logic__util__rigid_body_DEFAULT_COMPUTE_SHADER_LOCAL_SIZE(environment)

#define game_logic_ALLOWED_DISTANCE_CONSTRAINT_PENETRATION(environment) \
	0.05f//game_logic__util__spatial_FLOAT_FROM_METERS(environment, 0.05f)

#define game_logic_DISTANCE_CONSTRAINT_PENETRATION_VELOCITY_SCALE(environment) \
	/*0.05f*/0.01f//game_logic__util__spatial_FLOAT_FROM_METERS(environment, 0.05f)

#define game_logic_DISTANCE_CONSTRAINT_IMPULSE_SCALE(environment) 0.05f

#define game_logic_CURSOR_CONSTRAINT_VELOCITY_SCALE(environment) 0.03f
#define game_logic_CURSOR_CONSTRAINT_IMPULSE_SCALE(environment) 0.05f
#define game_logic_CURSOR_CONSTRAINT_MAX_IMPULSE(environment) 0.2f//0.5f

// TODO: Store separate masses for each body in buffer
//#define INVERSE_MASS 1.0f
//#define INVERSE_INERTIA 2.0f

// TODO: Make this depend on triangle materials
#define FRICTION_COEFFICIENT 0.5f

// To reduce compile times (due to driver bug), 
// should be disabled for release/performance testing
#define USE_DYNAMIC_SIZES true

// TODO: USE EXCLUSIVE OR IN PROXIMITY UTIL WHEN FLIPPING A BOOLEAN UNSIGNED INT

namespace game_logic
{
	template <unsigned int Vertex_Count, unsigned int Vertex_Index_Count>
	void create_model
	(
		game_environment::Environment& environment,
		GLuint const vertex_base_index,
		GLfloat (&vertices)[Vertex_Count][2u], 
		GLuint (&vertex_indices)[Vertex_Index_Count],
		Model<Vertex_Index_Count>& model
	)
	{
		for (GLuint i{ 0u }; i < Vertex_Index_Count; ++i)
		{
			model.vertex_indices[i] = vertex_base_index + vertex_indices[i];
		}

		model.inverse_mass = 0.0f;
		GLfloat center_x{ 0.0f };
		GLfloat center_y{ 0.0f };
		GLuint i{ 0u };
		while (i < Vertex_Index_Count)
		{
			GLuint const v0{ vertex_indices[i++] };
			GLuint const v1{ vertex_indices[i++] };
			GLuint const v2{ vertex_indices[i++] };

			GLfloat const x0{ vertices[v0][0u] };
			GLfloat const y0{ vertices[v0][1u] };

			GLfloat const x1{ vertices[v1][0u] };
			GLfloat const y1{ vertices[v1][1u] };

			GLfloat const x2{ vertices[v2][0u] };
			GLfloat const y2{ vertices[v2][1u] };

			// A - B: (x0 - x1, y0 - y1)
			// A - C: (x0 - x2, y0 - y2)
			GLfloat const weight{ std::abs((x0 - x1) * (y0 - y2) - (x0 - x2) * (y0 - y1)) };
			model.inverse_mass += weight;
			center_x += weight * (x0 + x1 + x2);
			center_y += weight * (y0 + y1 + y2);
		}
		model.inverse_mass = 1.0f / model.inverse_mass;
		center_x *= (1.0f / 3.0f) * model.inverse_mass;//(2.0f / 3.0f) * model.inverse_mass;
		center_y *= (1.0f / 3.0f) * model.inverse_mass;//(2.0f / 3.0f) * model.inverse_mass;
		model.inverse_mass *= 2.0f;

		for (i = 0u; i < Vertex_Count; ++i)
		{
			vertices[i][0u] -= center_x;
			vertices[i][1u] -= center_y;
			
			vertices[i][0u] *= game_logic__util__spatial_METER(environment);
			vertices[i][1u] *= game_logic__util__spatial_METER(environment);

			environment.state.GPU_buffers.rigid_bodies.triangles.vertices.values[vertex_base_index + i][0u] = vertices[i][0u];
			environment.state.GPU_buffers.rigid_bodies.triangles.vertices.values[vertex_base_index + i][1u] = vertices[i][1u];

			glClearNamedBufferSubData
			(
				environment.state.GPU_buffers.rigid_bodies.triangles.vertices.buffer,
				GL_RG32F, 
				environment.state.GPU_buffers.rigid_bodies.triangles.vertices.vertices_offset + (vertex_base_index + i) * environment.state.GPU_buffers.rigid_bodies.triangles.vertices.vertices_stride,
				sizeof(GLfloat[2u]), 
				GL_RG, GL_FLOAT, 
				&(vertices[i])
			);
		}

		model.inverse_inertia = 0.0f;
		i = 0u;
		while (i < Vertex_Index_Count)
		{
			GLuint const v0{ vertex_indices[i++] };
			GLuint const v1{ vertex_indices[i++] };
			GLuint const v2{ vertex_indices[i++] };

			GLfloat x0{ vertices[v0][0u] };
			x0 *= game_logic__util__spatial_METER_INVERSE(environment);
			GLfloat y0{ vertices[v0][1u] };
			y0 *= game_logic__util__spatial_METER_INVERSE(environment);

			GLfloat x1{ vertices[v1][0u] };
			x1 *= game_logic__util__spatial_METER_INVERSE(environment);
			GLfloat y1{ vertices[v1][1u] };
			y1 *= game_logic__util__spatial_METER_INVERSE(environment);

			GLfloat x2{ vertices[v2][0u] };
			x2 *= game_logic__util__spatial_METER_INVERSE(environment);
			GLfloat y2{ vertices[v2][1u] };
			y2 *= game_logic__util__spatial_METER_INVERSE(environment);

			GLfloat const triangle_center_x{ (1.0f / 3.0f) * (x0 + x1 + x2) };
			GLfloat const triangle_center_y{ (1.0f / 3.0f) * (y0 + y1 + y2) };

			x0 -= triangle_center_x;
			y0 -= triangle_center_y;

			x1 -= triangle_center_x;
			y1 -= triangle_center_y;

			x2 -= triangle_center_x;
			y2 -= triangle_center_y;

			GLfloat const triangle_mass{ 0.5f * std::abs((x0 - x1) * (y0 - y2) - (x0 - x2) * (y0 - y1)) };
			
			GLfloat const triangle_inertia
			{
				(
					(x0 * x0 + y0 * y0) +
					(x1 * x1 + y1 * y1) +
					(x2 * x2 + y2 * y2) +
					(x0 * x1 + y0 * y1) +
					(x1 * x2 + y1 * y2) +
					(x2 * x0 + y2 * y0)
				) * triangle_mass * (1.0f / 6.0f)
			};

			model.inverse_inertia += triangle_inertia + triangle_mass * (triangle_center_x * triangle_center_x + triangle_center_y * triangle_center_y);
		}
		model.inverse_inertia = 1.0f / model.inverse_inertia;
	}

	template <unsigned int Vertex_Index_Count>
	void instantiate_model
	(
		game_environment::Environment& environment,
		Model<Vertex_Index_Count>const& model,
		GLint const x, GLint const y, GLint const angle,
		GLint const x_velocity, GLint const y_velocity, GLint const angular_velocity
	)
	{
		GLint position[4u]{ x, y, angle, 0u };
		glClearNamedBufferSubData
		(
			environment.state.GPU_buffers.rigid_bodies.positions.buffer,
			GL_RGBA32I, 
			environment.state.GPU_buffers.rigid_bodies.positions.p_offset + environment.state.current_rigid_body_count * environment.state.GPU_buffers.rigid_bodies.positions.p_stride,
			sizeof(GLint[4u]), 
			GL_RGBA_INTEGER, GL_INT, 
			position
		);

		GLint velocity[4u]{ x_velocity, y_velocity, angular_velocity, 0u };
		glClearNamedBufferSubData
		(
			environment.state.GPU_buffers.rigid_bodies.velocities.buffer,
			GL_RGBA32I,
			environment.state.GPU_buffers.rigid_bodies.velocities.v_offset + environment.state.current_rigid_body_count * environment.state.GPU_buffers.rigid_bodies.velocities.v_stride,
			sizeof(GLint[4u]),
			GL_RGBA_INTEGER, GL_INT,
			velocity
		);

		GLfloat mass[2u]{ model.inverse_mass, model.inverse_inertia };
		glClearNamedBufferSubData
		(
			environment.state.body_masses_buffer,
			GL_RG32F,
			environment.state.body_masses_buffer_masses_offset + environment.state.current_rigid_body_count * environment.state.body_masses_buffer_masses_stride,
			sizeof(mass),
			GL_RG, GL_FLOAT,
			mass
		);

		GLuint i{ 0u };
		while (i < Vertex_Index_Count)
		{
			game_state::rigid_body::Triangle& triangle{ environment.state.GPU_buffers.rigid_bodies.triangles.values[environment.state.GPU_buffers.rigid_bodies.triangles.current_count] };
			triangle.vertices[0u] = model.vertex_indices[i++];
			triangle.vertices[1u] = model.vertex_indices[i++];
			triangle.vertices[2u] = model.vertex_indices[i++];
			triangle.body = environment.state.current_rigid_body_count;
			
			glClearNamedBufferSubData
			(
				environment.state.GPU_buffers.rigid_bodies.triangles.buffer,
				GL_RGBA32UI, 
				environment.state.GPU_buffers.rigid_bodies.triangles.triangles_offset + environment.state.GPU_buffers.rigid_bodies.triangles.current_count * environment.state.GPU_buffers.rigid_bodies.triangles.triangles_stride,
				sizeof(triangle), 
				GL_RGBA, GL_UNSIGNED_INT, 
				&triangle
			);

			GLuint const leaf_index{ game_logic_TRIANGLE_LEAFS_BASE_INDEX(environment) + environment.state.GPU_buffers.rigid_bodies.triangles.current_count };
			game_state::proximity::Bounding_Box& bounding_box
			{
				environment.state.proximity_tree.nodes[leaf_index].bounding_box
			};
			bounding_box.min.x = 0;
			bounding_box.min.y = 0;
			bounding_box.max.x = -1;
			bounding_box.max.y = -1;

			glClearNamedBufferSubData
			(
				environment.state.GPU_buffers.rigid_bodies.triangles.bounding_boxes.buffer,
				GL_RGBA32I,
				environment.state.GPU_buffers.rigid_bodies.triangles.bounding_boxes.boxes_offset + environment.state.GPU_buffers.rigid_bodies.triangles.current_count * environment.state.GPU_buffers.rigid_bodies.triangles.bounding_boxes.boxes_stride,
				sizeof(bounding_box),
				GL_RGBA, GL_INT,
				&bounding_box
			);

			util::proximity::insert_leaf_to_nonempty_tree(
				environment.state.proximity_tree, game_logic_MAX_LEAF_COUNT(environment),
				leaf_index,
				0, 0, -1, -1	// TODO: The bounding box is set twice!
			);

			++environment.state.GPU_buffers.rigid_bodies.triangles.current_count;
		}
		glClearNamedBufferSubData
		(
			environment.state.count_buffer, 
			GL_R32UI, 
			environment.state.count_buffer_triangles_offset, 
			sizeof(GLuint), 
			GL_RED_INTEGER, GL_UNSIGNED_INT, 
			&environment.state.GPU_buffers.rigid_bodies.triangles.current_count
		);

		++environment.state.current_rigid_body_count;
		glClearNamedBufferSubData
		(
			environment.state.count_buffer,
			GL_R32UI,
			environment.state.count_buffer_bodies_offset,
			sizeof(GLuint),
			GL_RED_INTEGER, GL_UNSIGNED_INT,
			&environment.state.current_rigid_body_count
		);
	}

	void print_capabilities(game_environment::Environment& environment)
	{
		std::cout << "OpenGL Capabilities:\n";

		GLint max_uniform_buffer_bindings;
		glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &max_uniform_buffer_bindings);
		std::cout << "Max uniform buffer bindings: " << max_uniform_buffer_bindings << '\n';

		GLint max_uniform_block_size;
		glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &max_uniform_block_size);
		std::cout << "Max uniform block size: " << max_uniform_block_size << '\n';

		GLint max_shader_storage_buffer_bindings;
		glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &max_shader_storage_buffer_bindings);
		std::cout << "Max shader storage buffer bindings: " << max_shader_storage_buffer_bindings << '\n';

		GLint max_shader_storage_block_size;
		glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &max_shader_storage_block_size);
		std::cout << "Max shader storage block size: " << max_shader_storage_block_size << '\n';

		GLint max_combined_shader_storage_blocks;
		glGetIntegerv(GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS, &max_combined_shader_storage_blocks);
		std::cout << "Max combined shader storage blocks: " << max_combined_shader_storage_blocks << '\n';

		GLint max_compute_shader_storage_blocks;
		glGetIntegerv(GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS, &max_compute_shader_storage_blocks);
		std::cout << "Max compute shader storage blocks: " << max_compute_shader_storage_blocks << '\n';

		GLint max_fragment_shader_storage_blocks;
		glGetIntegerv(GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS, &max_fragment_shader_storage_blocks);
		std::cout << "Max fragment shader storage blocks: " << max_fragment_shader_storage_blocks << '\n';

		GLint max_geometry_shader_storage_blocks;
		glGetIntegerv(GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS, &max_geometry_shader_storage_blocks);
		std::cout << "Max geometry shader storage blocks: " << max_geometry_shader_storage_blocks << '\n';

		GLint max_tess_control_shader_storage_blocks;
		glGetIntegerv(GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS, &max_tess_control_shader_storage_blocks);
		std::cout << "Max tess control shader storage blocks: " << max_tess_control_shader_storage_blocks << '\n';

		GLint max_tess_evaluation_shader_storage_blocks;
		glGetIntegerv(GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS, &max_tess_evaluation_shader_storage_blocks);
		std::cout << "Max tess evaluation shader storage blocks: " << max_tess_evaluation_shader_storage_blocks << '\n';

		GLint max_vertex_shader_storage_blocks;
		glGetIntegerv(GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS, &max_vertex_shader_storage_blocks);
		std::cout << "Max vertex shader storage blocks: " << max_vertex_shader_storage_blocks << '\n';

		std::cout << std::endl;
	}

	void adapt_to_default_framebuffer_size
	(
		game_environment::Environment& environment, 
		GLint const width, GLint const height
	)
	{
		glViewport(0, 0, width, height);

		// TODO: Make sure supersampling is not used for the default framebuffer. Otherwise, we should 
		// make the fluid texture use supersampling and resolve (downscale) it manually to another texture.
		glCreateTextures(GL_TEXTURE_2D, 1u, &environment.state.fluid_texture);
		glTextureParameteri(environment.state.fluid_texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(environment.state.fluid_texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTextureStorage2D(environment.state.fluid_texture, 1u, GL_RGBA32F, width, height);

		glBindTextureUnit(0u, environment.state.fluid_texture);

		glCreateFramebuffers(1u, &environment.state.fluid_framebuffer);
		glNamedFramebufferTexture(environment.state.fluid_framebuffer, GL_COLOR_ATTACHMENT0, environment.state.fluid_texture, 0);
		GLenum const draw_buffers[]{ GL_NONE, GL_COLOR_ATTACHMENT0 };
		glNamedFramebufferDrawBuffers(environment.state.fluid_framebuffer, std::size(draw_buffers), draw_buffers);

		GLenum framebuffer_status = glCheckNamedFramebufferStatus(environment.state.fluid_framebuffer, GL_DRAW_FRAMEBUFFER);
		if (framebuffer_status != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cerr << "Fluid framebuffer not completed, status code: " << framebuffer_status << std::endl;
		}
	}

	void delete_fluid_texture_and_framebuffer(game_environment::Environment& environment)
	{
		glDeleteTextures(1u, &environment.state.fluid_texture);
		glDeleteFramebuffers(1u, &environment.state.fluid_framebuffer);
	}

	void initialize(game_environment::Environment& environment)
	{
		std::cout << "Initializing...\n" << std::endl;

		std::cout << "Max fluid particle count: " << game_logic_MAX_FLUID_PARTICLE_COUNT(environment) << '\n';
		std::cout << "Max rigid body count: " << game_logic_MAX_RIGID_BODY_COUNT(environment) << '\n';
		std::cout << "Max triangle count: " << game_logic_MAX_TRIANGLE_COUNT(environment) << '\n';
		std::cout << "Max vertex count: " << game_logic_MAX_VERTEX_COUNT(environment) << '\n';
		std::cout << "Max leaf count: " << game_logic_MAX_LEAF_COUNT(environment) << " (Capacity: " << 0b01111111111111111111111111111111u << ")\n";
		std::cout << "Max triangle contact count: " << game_logic_MAX_TRIANGLE_CONTACT_COUNT(environment) << "\n" << std::endl;

		print_capabilities(environment);

		glCreateQueries(GL_TIME_ELAPSED, 1, &environment.state.time_elapsed_query);

		environment.state.GPU_buffers.rigid_bodies.triangles.vertices.values = new GLfloat[game_logic_MAX_VERTEX_COUNT(environment)][2u];

		glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
		glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

		int width, height;
		glfwGetFramebufferSize(environment.window, &width, &height);
		adapt_to_default_framebuffer_size(environment, width, height);

		glEnablei(GL_BLEND, 1);
		glBlendEquationSeparatei(1, GL_FUNC_ADD, GL_FUNC_ADD);
		glBlendFuncSeparatei(1, GL_SRC_ALPHA, GL_ONE, GL_ONE, GL_ONE);

		environment.state.debug_fluid_particles_visible = false;
		environment.state.triangle_wireframes_visible = false;
		environment.state.rigid_bodies_visible = false;
		environment.state.triangle_normals_visible = false;
		environment.state.leaf_bounding_boxes_visible = false;
		environment.state.parent_bounding_boxes_visible = false;
		environment.state.leaf_contacts_visible = false;
		environment.state.contact_point_positions_visible = false;
		environment.state.contact_basis_visible = false;
		environment.state.contact_impulses_visible = false;
		environment.state.gravity_visible = false;

		// TODO: Use glBindBuffersBase (note the s) for binding multiple buffers at once
		// IMPORTANT TODO: We do not need to do a position snapshot if velocity-based position correction 
		// is sufficient.

		// IMPORTANT TODO: Make sure to not exceed GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS
		// which is only guaranteed to be at least 8 (which we are currently exceeding).
		// Do the same for GL_MAX_UNIFORM_BUFFER_BINDINGS, but that has a lower limit of 
		// 36.

		char const* max_rigid_body_count_definition;
		char const* max_triangle_count_definition;
		char const* max_vertex_count_definition;
		char const* max_contact_count_definition;
		char const* max_distance_constraint_count_definition;
		char const* max_fluid_particle_count_definition;
		char const* max_fluid_contact_count_definition;
		char const* max_fluid_triangle_contact_count_definition;
#if USE_DYNAMIC_SIZES == true
		max_rigid_body_count_definition = util_shader_DEFINE("MAX_RIGID_BODY_COUNT", "");
		max_triangle_count_definition = util_shader_DEFINE("MAX_TRIANGLE_COUNT", "");
		max_vertex_count_definition = util_shader_DEFINE("MAX_VERTEX_COUNT", "");
		max_contact_count_definition = util_shader_DEFINE("MAX_CONTACT_COUNT", "");
		max_distance_constraint_count_definition = util_shader_DEFINE("MAX_DISTANCE_CONSTRAINT_COUNT", "");
		max_fluid_particle_count_definition = util_shader_DEFINE("MAX_FLUID_PARTICLE_COUNT", "");
		max_fluid_contact_count_definition = util_shader_DEFINE("MAX_FLUID_CONTACT_COUNT", "");
		max_fluid_triangle_contact_count_definition = util_shader_DEFINE("MAX_FLUID_TRIANGLE_CONTACT_COUNT", "");
#else
		max_rigid_body_count_definition = util_shader_DEFINE("MAX_RIGID_BODY_COUNT", STRINGIFY(game_logic_MAX_RIGID_BODY_COUNT(environment)));
		max_triangle_count_definition = util_shader_DEFINE("MAX_TRIANGLE_COUNT", STRINGIFY(game_logic_MAX_TRIANGLE_COUNT(environment)));
		max_vertex_count_definition = util_shader_DEFINE("MAX_VERTEX_COUNT", STRINGIFY(game_logic_MAX_VERTEX_COUNT(environment)));
		max_contact_count_definition = util_shader_DEFINE("MAX_CONTACT_COUNT", STRINGIFY(game_logic_MAX_TRIANGLE_CONTACT_COUNT(environment)));
		max_distance_constraint_count_definition = util_shader_DEFINE("MAX_DISTANCE_CONSTRAINT_COUNT", STRINGIFY(game_logic_MAX_DISTANCE_CONSTRAINT_COUNT(environment)));
		max_fluid_particle_count_definition = util_shader_DEFINE("MAX_FLUID_PARTICLE_COUNT", STRINGIFY(game_logic_MAX_FLUID_PARTICLE_COUNT(environment)));
		max_fluid_contact_count_definition = util_shader_DEFINE("MAX_FLUID_CONTACT_COUNT", STRINGIFY(MAX_FLUID_CONTACT_COUNT(environment)));
		max_fluid_triangle_contact_count_definition = util_shader_DEFINE("MAX_FLUID_TRIANGLE_CONTACT_COUNT", STRINGIFY(MAX_FLUID_TRIANGLE_CONTACT_COUNT(environment)));
#endif

		environment.state.tick = 0u;
		environment.state.physics_running = true;

		environment.state.grabbed_triangle = game_logic__util__proximity_NULL_INDEX;
		environment.state.distance_constraint_start_triangle = game_logic__util__proximity_NULL_INDEX;

		cursor_types::initialize(environment);

		environment.state.camera.xy.x = 100 * 1000000;
		environment.state.camera.xy.y = 100 * 1000000;
		environment.state.camera.angle = 10600000;
		environment.state.camera.z = game_logic__util__spatial_FLOAT_FROM_METERS(environment, 2.0f);
		environment.state.camera.view_rotation.column_0[0] = 1.0f;
		environment.state.camera.view_rotation.column_0[1] = 0.0f;
		environment.state.camera.view_rotation.column_1[0] = 0.0f;
		environment.state.camera.view_rotation.column_1[1] = 1.0f;
		
		environment.state.point_grabbed = false;
		environment.state.grabbed_gravity_source = game_logic__util__proximity_NULL_INDEX;

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
		std::cout << "Dummy shader compiled" << std::endl;

		// TODO: This vertex shader compilation should be done ONCE
		::util::shader::set_shader_statically
		(
			vertex_shader,
			util_shader_VERSION,
			::util::shader::file_to_string("util/full_screen.vert")
		);
		::util::shader::set_shader_statically
		(
			fragment_shader,
			util_shader_VERSION,
			::util::shader::file_to_string("util/fluid.frag")
		);
		environment.state.fluid_draw_shader = ::util::shader::create_program(vertex_shader, fragment_shader);
		environment.state.fluid_draw_shader_fluid_texture_uniform_location = glGetUniformLocation(environment.state.fluid_draw_shader, "fluid_texture");
		glProgramUniform1i
		(
			environment.state.fluid_draw_shader, 
			environment.state.fluid_draw_shader_fluid_texture_uniform_location, 
			0
		);
		std::cout << "Fluid draw shader compiled. Fluid texture uniform location: " << environment.state.fluid_draw_shader_fluid_texture_uniform_location << std::endl;

		::util::shader::set_shader_statically
		(
			vertex_shader,
			util_shader_VERSION,
			util_shader_DEFINE("METER", STRINGIFY(game_logic__util__spatial_METER(environment))),
			util_shader_DEFINE("RADIUS", STRINGIFY(game_logic_FLUID_PARTICLE_DRAW_RADIUS(environment))),
			max_fluid_particle_count_definition,
			util_shader_DEFINE("FLUID_POSITION_BINDING", STRINGIFY(game_logic__util_FLUID_POSITION_BINDING)),
			util_shader_DEFINE("FLUID_VELOCITY_BINDING", STRINGIFY(game_logic__util_FLUID_VELOCITY_BINDING)),
			util_shader_DEFINE("CAMERA_BINDING", STRINGIFY(game_CAMERA_BINDING)),
			game_PROJECTION_SCALE_DEFINITION(environment),
			::util::shader::file_to_string("util/fluid_particles.vert")
		);
		::util::shader::set_shader_statically
		(
			fragment_shader,
			util_shader_VERSION,
			util_shader_DEFINE("METER", STRINGIFY(game_logic__util__spatial_METER(environment))),
			util_shader_DEFINE("RADIUS", STRINGIFY(game_logic_FLUID_PARTICLE_DRAW_RADIUS(environment))),
			::util::shader::file_to_string("util/fluid_particles.frag")
		);
		environment.state.fluid_particles_draw_shader = ::util::shader::create_program(vertex_shader, fragment_shader);
		std::cout << "Fluid particles draw shader compiled" << std::endl;

		::util::shader::set_shader_statically
		(
			vertex_shader,
			util_shader_VERSION,
			util_shader_DEFINE("METER", STRINGIFY(game_logic__util__spatial_METER(environment))),
			util_shader_DEFINE("RADIUS", STRINGIFY(game_logic_FLUID_PARTICLE_DRAW_RADIUS(environment))),
			max_fluid_particle_count_definition,
			util_shader_DEFINE("FLUID_POSITION_BINDING", STRINGIFY(game_logic__util_FLUID_POSITION_BINDING)),
			util_shader_DEFINE("FLUID_VELOCITY_BINDING", STRINGIFY(game_logic__util_FLUID_VELOCITY_BINDING)),
			util_shader_DEFINE("CAMERA_BINDING", STRINGIFY(game_CAMERA_BINDING)),
			game_PROJECTION_SCALE_DEFINITION(environment),
			::util::shader::file_to_string("util/debug_fluid_particles.vert")
		);
		::util::shader::set_shader_statically
		(
			fragment_shader,
			util_shader_VERSION,
			util_shader_DEFINE("COLOR", "vec4(1.0, 1.0, 1.0, 1.0)"),
			::util::shader::file_to_string("util/static_color.frag")
		);
		environment.state.debug_fluid_particles_draw_shader = ::util::shader::create_program(vertex_shader, fragment_shader);
		std::cout << "Debug fluid particles draw shader compiled" << std::endl;

		::util::shader::set_shader_statically
		(
			vertex_shader,
			util_shader_VERSION,
			game_PROJECTION_SCALE_DEFINITION(environment),
			max_rigid_body_count_definition,
			util_shader_DEFINE("CAMERA_BINDING", STRINGIFY(game_CAMERA_BINDING)),
			util_shader_DEFINE("POSITION_BINDING", STRINGIFY(game_logic__util_RIGID_BODY_POSITION_BINDING)),
			util_shader_DEFINE("METER", STRINGIFY(game_logic__util__spatial_METER(environment))), 
			util_shader_DEFINE("RADIAN_INVERSE", STRINGIFY(game_logic__util__spatial_RADIAN_INVERSE(environment))), 
			::util::shader::file_to_string("util/rigid_body_debug.vert")
		);
		::util::shader::set_shader_statically
		(
			fragment_shader,
			util_shader_VERSION,
			util_shader_DEFINE("COLOR", "vec4(1.0, 1.0, 1.0, 1.0)"), 
			::util::shader::file_to_string("util/static_color.frag")
		);
		environment.state.rigid_body_debug_rendering_shader = ::util::shader::create_program(vertex_shader, fragment_shader);
		std::cout << "Rigid body debug rendering shader compiled" << std::endl;

		::util::shader::set_shader_statically
		(
			vertex_shader,
			util_shader_VERSION,
			game_PROJECTION_SCALE_DEFINITION(environment),
			max_triangle_count_definition,
			max_vertex_count_definition,
			max_rigid_body_count_definition,
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
			util_shader_DEFINE("COLOR", "vec4(0.3, 0.3, 0.3, 0.5)"), // TODO: REMOVE ALPHA
			::util::shader::file_to_string("util/static_color.frag") // TODO: Should only be done once
		);
		environment.state.triangle_draw_shader = ::util::shader::create_program(vertex_shader, fragment_shader);
		std::cout << "Triangle draw shader compiled" << std::endl;

		::util::shader::set_shader_statically
		(
			vertex_shader,
			util_shader_VERSION,
			game_PROJECTION_SCALE_DEFINITION(environment),
			max_triangle_count_definition,
			max_vertex_count_definition,
			max_rigid_body_count_definition,
			util_shader_DEFINE("CAMERA_BINDING", STRINGIFY(game_CAMERA_BINDING)),
			util_shader_DEFINE("POSITION_BINDING", STRINGIFY(game_logic__util_RIGID_BODY_POSITION_BINDING)),
			util_shader_DEFINE("TRIANGLE_BINDING", STRINGIFY(game_logic__util_TRIANGLE_BINDING)),
			util_shader_DEFINE("VERTEX_BINDING", STRINGIFY(game_logic__util_VERTEX_BINDING)),
			util_shader_DEFINE("METER", STRINGIFY(game_logic__util__spatial_METER(environment))), // TODO: Remove
			util_shader_DEFINE("RADIAN_INVERSE", STRINGIFY(game_logic__util__spatial_RADIAN_INVERSE(environment))),
			::util::shader::file_to_string("util/triangle_wireframes.vert")
		);
		::util::shader::set_shader_statically
		(
			fragment_shader,
			util_shader_VERSION,
			util_shader_DEFINE("COLOR", "vec4(0.8, 0.8, 0.8, 1.0)"),
			::util::shader::file_to_string("util/static_color.frag") // TODO: Should only be done once
		);
		environment.state.triangle_wireframes_draw_shader = ::util::shader::create_program(vertex_shader, fragment_shader);
		std::cout << "Triangle wireframes draw shader compiled" << std::endl;

		::util::shader::set_shader_statically
		(
			vertex_shader,
			util_shader_VERSION,
			game_PROJECTION_SCALE_DEFINITION(environment),
			max_triangle_count_definition,
			max_vertex_count_definition,
			max_rigid_body_count_definition,
			util_shader_DEFINE("CAMERA_BINDING", STRINGIFY(game_CAMERA_BINDING)),
			util_shader_DEFINE("POSITION_BINDING", STRINGIFY(game_logic__util_RIGID_BODY_POSITION_BINDING)),
			util_shader_DEFINE("TRIANGLE_BINDING", STRINGIFY(game_logic__util_TRIANGLE_BINDING)),
			util_shader_DEFINE("VERTEX_BINDING", STRINGIFY(game_logic__util_VERTEX_BINDING)),
			util_shader_DEFINE("METER", STRINGIFY(game_logic__util__spatial_METER(environment))), // TODO: Remove
			util_shader_DEFINE("RADIAN_INVERSE", STRINGIFY(game_logic__util__spatial_RADIAN_INVERSE(environment))),
			::util::shader::file_to_string("util/hovered_triangle_wireframe.vert")
		);
		::util::shader::set_shader_statically
		(
			fragment_shader,
			util_shader_VERSION,
			util_shader_DEFINE("COLOR", "vec4(1.0, 1.0, 0.0, 1.0)"),
			::util::shader::file_to_string("util/static_color.frag") // TODO: Should only be done once
		);
		environment.state.hovered_triangle_wireframe_draw_shader = ::util::shader::create_program(vertex_shader, fragment_shader);
		environment.state.hovered_triangle_wireframe_hovered_triangle_uniform_location = glGetUniformLocation
		(
			environment.state.hovered_triangle_wireframe_draw_shader, "hovered_triangle"
		);
		std::cout << "Hovered triangle wireframe draw shader compiled" << std::endl;

		::util::shader::set_shader_statically
		(
			vertex_shader,
			util_shader_VERSION,
			game_PROJECTION_SCALE_DEFINITION(environment),
			max_triangle_count_definition,
			max_vertex_count_definition,
			max_rigid_body_count_definition,
			util_shader_DEFINE("CAMERA_BINDING", STRINGIFY(game_CAMERA_BINDING)),
			util_shader_DEFINE("POSITION_BINDING", STRINGIFY(game_logic__util_RIGID_BODY_POSITION_BINDING)),
			util_shader_DEFINE("TRIANGLE_BINDING", STRINGIFY(game_logic__util_TRIANGLE_BINDING)),
			util_shader_DEFINE("VERTEX_BINDING", STRINGIFY(game_logic__util_VERTEX_BINDING)),
			util_shader_DEFINE("METER", STRINGIFY(game_logic__util__spatial_METER(environment))), // TODO: Remove
			util_shader_DEFINE("RADIAN_INVERSE", STRINGIFY(game_logic__util__spatial_RADIAN_INVERSE(environment))),
			::util::shader::file_to_string("util/triangle_normals.vert")
		);
		::util::shader::set_shader_statically
		(
			fragment_shader,
			util_shader_VERSION,
			util_shader_DEFINE("COLOR", "vec4(1.0, 1.0, 0.0, 1.0)"),
			::util::shader::file_to_string("util/static_color.frag") // TODO: Should only be done once
		);
		environment.state.triangle_normal_draw_shader = ::util::shader::create_program(vertex_shader, fragment_shader);
		std::cout << "Triangle normal draw shader compiled" << std::endl;

		::util::shader::set_shader_statically
		(
			vertex_shader,
			util_shader_VERSION,
			game_PROJECTION_SCALE_DEFINITION(environment),
			max_triangle_count_definition,
			max_vertex_count_definition,
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
		std::cout << "Triangle bounding box draw shader compiled" << std::endl;

		::util::shader::set_shader_statically
		(
			vertex_shader,
			util_shader_VERSION,
			game_PROJECTION_SCALE_DEFINITION(environment),
			max_fluid_particle_count_definition,
			max_vertex_count_definition,
			util_shader_DEFINE("FLUID_BOUNDING_BOX_BINDING", STRINGIFY(game_logic__util_FLUID_BOUNDING_BOX_BINDING)),
			util_shader_DEFINE("CAMERA_BINDING", STRINGIFY(game_CAMERA_BINDING)),
			::util::shader::file_to_string("util/fluid_bounding_boxes.vert")
		);
		::util::shader::set_shader_statically
		(
			fragment_shader,
			util_shader_VERSION,
			util_shader_DEFINE("COLOR", "vec4(1.0, 1.0, 1.0, 1.0)"),
			::util::shader::file_to_string("util/static_color.frag") // TODO: Should only be done once
		);
		environment.state.fluid_bounding_boxes_draw_shader = ::util::shader::create_program(vertex_shader, fragment_shader);
		std::cout << "Fluid bounding boxes draw shader compiled" << std::endl;

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
		std::cout << "Parent bounding box draw shader compiled" << std::endl;

		::util::shader::set_shader_statically
		(
			vertex_shader,
			util_shader_VERSION,
			util_shader_DEFINE("FLUID_CONTACT_COUNT_BINDING", STRINGIFY(game_logic__util_FLUID_CONTACT_COUNT_BINDING)),
			util_shader_DEFINE("FLUID_CONTACT_BINDING", STRINGIFY(game_logic__util_FLUID_CONTACT_BINDING)),
			max_fluid_contact_count_definition,
			max_fluid_particle_count_definition,
			util_shader_DEFINE("FLUID_POSITION_BINDING", STRINGIFY(game_logic__util_FLUID_POSITION_BINDING)),
			util_shader_DEFINE("FLUID_VELOCITY_BINDING", STRINGIFY(game_logic__util_FLUID_VELOCITY_BINDING)),
			util_shader_DEFINE("LOCAL_SIZE", STRINGIFY(PERSIST_FLUID_CONTACT_LOCAL_SIZE(environment))),
			util_shader_DEFINE("INVERSE_MASS", STRINGIFY(FLUID_INVERSE_MASS(environment))),
			util_shader_DEFINE("STRENGTH_RADIUS", STRINGIFY(FLUID_STRENGTH_RADIUS(environment))),
			util_shader_DEFINE("MAX_STRENGTH", STRINGIFY(FLUID_MAX_STRENGTH(environment))),
			util_shader_DEFINE("TARGET_RADIUS", STRINGIFY(FLUID_TARGET_RADIUS(environment))),
			util_shader_DEFINE("METER_INVERSE", STRINGIFY(game_logic__util__spatial_METER_INVERSE(environment))),
			util_shader_DEFINE("METER", STRINGIFY(game_logic__util__spatial_METER(environment))),
			util_shader_DEFINE("BOUNDING_BOX_BINDING", STRINGIFY(game_logic__util_TRIANGLE_BOUNDING_BOX_BINDING)),
			util_shader_DEFINE("CAMERA_BINDING", STRINGIFY(game_CAMERA_BINDING)),
			game_PROJECTION_SCALE_DEFINITION(environment),
			::util::shader::file_to_string("util/fluid_leaf_contacts.vert")
		);
		::util::shader::set_shader_statically
		(
			fragment_shader,
			util_shader_VERSION,
			util_shader_DEFINE("COLOR", "vec4(0.0, 1.0, 0.0, 1.0)"),
			::util::shader::file_to_string("util/static_color.frag") // TODO: Should only be done once
		);
		environment.state.fluid_leaf_contacts_draw_shader = ::util::shader::create_program(vertex_shader, fragment_shader);
		std::cout << "Fluid leaf contacts draw shader compiled" << std::endl;

		::util::shader::set_shader_statically
		(
			vertex_shader,
			util_shader_VERSION,
			util_shader_DEFINE("FLUID_TRIANGLE_CONTACT_COUNT_BINDING", STRINGIFY(game_logic__util_FLUID_TRIANGLE_CONTACT_COUNT_BINDING)),
			util_shader_DEFINE("FLUID_TRIANGLE_CONTACT_BINDING", STRINGIFY(game_logic__util_FLUID_TRIANGLE_CONTACT_BINDING)),
			max_fluid_triangle_contact_count_definition,
			max_fluid_particle_count_definition,
			util_shader_DEFINE("POSITION_BINDING", STRINGIFY(game_logic__util_RIGID_BODY_POSITION_BINDING)),
			max_rigid_body_count_definition,
			util_shader_DEFINE("TRIANGLE_BINDING", STRINGIFY(game_logic__util_TRIANGLE_BINDING)),
			max_triangle_count_definition,
			util_shader_DEFINE("VERTEX_BINDING", STRINGIFY(game_logic__util_VERTEX_BINDING)),
			max_vertex_count_definition,
			util_shader_DEFINE("FLUID_POSITION_BINDING", STRINGIFY(game_logic__util_FLUID_POSITION_BINDING)),
			util_shader_DEFINE("FLUID_VELOCITY_BINDING", STRINGIFY(game_logic__util_FLUID_VELOCITY_BINDING)),
			util_shader_DEFINE("RADIAN_INVERSE", STRINGIFY(game_logic__util__spatial_RADIAN_INVERSE(environment))),
			util_shader_DEFINE("INVERSE_MASS", STRINGIFY(FLUID_INVERSE_MASS(environment))),
			util_shader_DEFINE("RADIUS", STRINGIFY(FLUID_TRIANGLE_RADIUS(environment))),
			util_shader_DEFINE("PARTICLE_INVERSE_MASS", STRINGIFY(FLUID_INVERSE_MASS(environment))),
			util_shader_DEFINE("BODY_MASSES_BINDING", STRINGIFY(game_logic__util_BODY_MASSES_BINDING)),
			//util_shader_DEFINE("BODY_INVERSE_MASS", STRINGIFY(INVERSE_MASS)),
			//util_shader_DEFINE("BODY_INVERSE_INERTIA", STRINGIFY(INVERSE_INERTIA)),
			util_shader_DEFINE("METER_INVERSE", STRINGIFY(game_logic__util__spatial_METER_INVERSE(environment))),
			util_shader_DEFINE("METER", STRINGIFY(game_logic__util__spatial_METER(environment))),
			util_shader_DEFINE("CAMERA_BINDING", STRINGIFY(game_CAMERA_BINDING)),
			game_PROJECTION_SCALE_DEFINITION(environment),
			::util::shader::file_to_string("util/fluid_triangle_leaf_contacts.vert")
		);
		::util::shader::set_shader_statically
		(
			fragment_shader,
			util_shader_VERSION,
			util_shader_DEFINE("COLOR", "vec4(0.0, 1.0, 0.0, 1.0)"),
			::util::shader::file_to_string("util/static_color.frag") // TODO: Should only be done once
		);
		environment.state.fluid_triangle_leaf_contacts_draw_shader = ::util::shader::create_program(vertex_shader, fragment_shader);
		std::cout << "Fluid triangle leaf contacts draw shader compiled" << std::endl;

		::util::shader::set_shader_statically
		(
			vertex_shader,
			util_shader_VERSION,
			util_shader_DEFINE("CONTACT_BINDING", STRINGIFY(game_logic__util_CONTACT_BINDING)),
			max_contact_count_definition,
			max_triangle_count_definition,
			max_vertex_count_definition,
			max_rigid_body_count_definition,
			util_shader_DEFINE("POSITION_BINDING", STRINGIFY(game_logic__util_RIGID_BODY_POSITION_BINDING)),
			util_shader_DEFINE("TRIANGLE_BINDING", STRINGIFY(game_logic__util_TRIANGLE_BINDING)),
			util_shader_DEFINE("VERTEX_BINDING", STRINGIFY(game_logic__util_VERTEX_BINDING)),
			util_shader_DEFINE("VELOCITY_BINDING", STRINGIFY(game_logic__util_RIGID_BODY_VELOCITY_BINDING)),
			util_shader_DEFINE("RADIAN_INVERSE", STRINGIFY(game_logic__util__spatial_RADIAN_INVERSE(environment))),
			util_shader_DEFINE("BOUNDING_BOX_BINDING", STRINGIFY(game_logic__util_TRIANGLE_BOUNDING_BOX_BINDING)),
			util_shader_DEFINE("CAMERA_BINDING", STRINGIFY(game_CAMERA_BINDING)),
			game_PROJECTION_SCALE_DEFINITION(environment),
			::util::shader::file_to_string("util/leaf_contact.vert")
		);
		::util::shader::set_shader_statically
		(
			fragment_shader,
			util_shader_VERSION,
			util_shader_DEFINE("COLOR", "vec4(0.0, 1.0, 0.0, 1.0)"),
			::util::shader::file_to_string("util/static_color.frag") // TODO: Should only be done once
		);
		environment.state.leaf_triangle_contact_draw_shader = ::util::shader::create_program(vertex_shader, fragment_shader);
		std::cout << "Leaf contact draw shader compiled" << std::endl;

		::util::shader::set_shader_statically
		(
			vertex_shader,
			util_shader_VERSION,
			util_shader_DEFINE("CONTACT_BINDING", STRINGIFY(game_logic__util_CONTACT_BINDING)),
			util_shader_DEFINE("CONTACT_SURFACE_BINDING", STRINGIFY(game_logic__util_CONTACT_SURFACE_BINDING)),
			max_contact_count_definition,
			max_rigid_body_count_definition,
			util_shader_DEFINE("POSITION_BINDING", STRINGIFY(game_logic__util_RIGID_BODY_POSITION_BINDING)),
			util_shader_DEFINE("CAMERA_BINDING", STRINGIFY(game_CAMERA_BINDING)),
			game_PROJECTION_SCALE_DEFINITION(environment),
			::util::shader::file_to_string("util/contact_point_offsets.vert")
		);
		::util::shader::set_shader_statically
		(
			fragment_shader,
			util_shader_VERSION,
			util_shader_DEFINE("COLOR", "vec4(0.0, 1.0, 1.0, 1.0)"),
			::util::shader::file_to_string("util/static_color.frag") // TODO: Should only be done once
		);
		environment.state.contact_point_offsets_draw_shader = ::util::shader::create_program(vertex_shader, fragment_shader);
		std::cout << "Contact point offsets draw shader compiled" << std::endl;

		::util::shader::set_shader_statically
		(
			vertex_shader,
			util_shader_VERSION,
			util_shader_DEFINE("FLUID_TRIANGLE_CONTACT_COUNT_BINDING", STRINGIFY(game_logic__util_FLUID_TRIANGLE_CONTACT_COUNT_BINDING)),
			util_shader_DEFINE("FLUID_TRIANGLE_CONTACT_BINDING", STRINGIFY(game_logic__util_FLUID_TRIANGLE_CONTACT_BINDING)),
			max_fluid_triangle_contact_count_definition,
			max_fluid_particle_count_definition,
			util_shader_DEFINE("POSITION_BINDING", STRINGIFY(game_logic__util_RIGID_BODY_POSITION_BINDING)),
			max_rigid_body_count_definition,
			util_shader_DEFINE("TRIANGLE_BINDING", STRINGIFY(game_logic__util_TRIANGLE_BINDING)),
			max_triangle_count_definition,
			util_shader_DEFINE("VERTEX_BINDING", STRINGIFY(game_logic__util_VERTEX_BINDING)),
			max_vertex_count_definition,
			util_shader_DEFINE("FLUID_POSITION_BINDING", STRINGIFY(game_logic__util_FLUID_POSITION_BINDING)),
			util_shader_DEFINE("FLUID_VELOCITY_BINDING", STRINGIFY(game_logic__util_FLUID_VELOCITY_BINDING)),
			util_shader_DEFINE("RADIAN_INVERSE", STRINGIFY(game_logic__util__spatial_RADIAN_INVERSE(environment))),
			//util_shader_DEFINE("INVERSE_MASS", STRINGIFY(FLUID_INVERSE_MASS(environment))),
			util_shader_DEFINE("RADIUS", STRINGIFY(FLUID_TRIANGLE_RADIUS(environment))),
			util_shader_DEFINE("PARTICLE_INVERSE_MASS", STRINGIFY(FLUID_INVERSE_MASS(environment))),
			util_shader_DEFINE("BODY_MASSES_BINDING", STRINGIFY(game_logic__util_BODY_MASSES_BINDING)),
			//util_shader_DEFINE("BODY_INVERSE_MASS", STRINGIFY(INVERSE_MASS)),
			//util_shader_DEFINE("BODY_INVERSE_INERTIA", STRINGIFY(INVERSE_INERTIA)),
			util_shader_DEFINE("METER_INVERSE", STRINGIFY(game_logic__util__spatial_METER_INVERSE(environment))),
			util_shader_DEFINE("METER", STRINGIFY(game_logic__util__spatial_METER(environment))),
			util_shader_DEFINE("CAMERA_BINDING", STRINGIFY(game_CAMERA_BINDING)),
			game_PROJECTION_SCALE_DEFINITION(environment),
			::util::shader::file_to_string("util/fluid_triangle_contact_points.vert")
		);
		::util::shader::set_shader_statically
		(
			fragment_shader,
			util_shader_VERSION,
			util_shader_DEFINE("COLOR", "vec4(0.0, 1.0, 1.0, 1.0)"),
			::util::shader::file_to_string("util/static_color.frag") // TODO: Should only be done once
		);
		environment.state.fluid_triangle_contact_points_draw_shader = ::util::shader::create_program(vertex_shader, fragment_shader);
		std::cout << "Fluid triangle contact points draw shader compiled" << std::endl;

		::util::shader::set_shader_statically
		(
			vertex_shader,
			util_shader_VERSION,
			util_shader_DEFINE("CONTACT_BINDING", STRINGIFY(game_logic__util_CONTACT_BINDING)),
			util_shader_DEFINE("CONTACT_SURFACE_BINDING", STRINGIFY(game_logic__util_CONTACT_SURFACE_BINDING)),
			max_contact_count_definition,
			max_rigid_body_count_definition,
			util_shader_DEFINE("POSITION_BINDING", STRINGIFY(game_logic__util_RIGID_BODY_POSITION_BINDING)),
			util_shader_DEFINE("CAMERA_BINDING", STRINGIFY(game_CAMERA_BINDING)),
			util_shader_DEFINE("METER_INVERSE", STRINGIFY(game_logic__util__spatial_METER_INVERSE(environment))),
			util_shader_DEFINE("METER", STRINGIFY(game_logic__util__spatial_METER(environment))),
			game_PROJECTION_SCALE_DEFINITION(environment),
			::util::shader::file_to_string("util/contact_point_positions.vert")
		);
		::util::shader::set_shader_statically
		(
			fragment_shader,
			util_shader_VERSION,
			util_shader_DEFINE("COLOR", "vec4(0.0, 1.0, 1.0, 1.0)"),
			::util::shader::file_to_string("util/static_color.frag") // TODO: Should only be done once
		);
		environment.state.contact_point_positions_draw_shader = ::util::shader::create_program(vertex_shader, fragment_shader);
		std::cout << "Contact point positions draw shader compiled" << std::endl;

		::util::shader::set_shader_statically
		(
			vertex_shader,
			util_shader_VERSION,
			util_shader_DEFINE("FLUID_TRIANGLE_CONTACT_COUNT_BINDING", STRINGIFY(game_logic__util_FLUID_TRIANGLE_CONTACT_COUNT_BINDING)),
			util_shader_DEFINE("FLUID_TRIANGLE_CONTACT_BINDING", STRINGIFY(game_logic__util_FLUID_TRIANGLE_CONTACT_BINDING)),
			max_fluid_triangle_contact_count_definition,
			max_fluid_particle_count_definition,
			util_shader_DEFINE("POSITION_BINDING", STRINGIFY(game_logic__util_RIGID_BODY_POSITION_BINDING)),
			max_rigid_body_count_definition,
			util_shader_DEFINE("TRIANGLE_BINDING", STRINGIFY(game_logic__util_TRIANGLE_BINDING)),
			max_triangle_count_definition,
			util_shader_DEFINE("VERTEX_BINDING", STRINGIFY(game_logic__util_VERTEX_BINDING)),
			max_vertex_count_definition,
			util_shader_DEFINE("FLUID_POSITION_BINDING", STRINGIFY(game_logic__util_FLUID_POSITION_BINDING)),
			util_shader_DEFINE("FLUID_VELOCITY_BINDING", STRINGIFY(game_logic__util_FLUID_VELOCITY_BINDING)),
			util_shader_DEFINE("RADIAN_INVERSE", STRINGIFY(game_logic__util__spatial_RADIAN_INVERSE(environment))),
			//util_shader_DEFINE("INVERSE_MASS", STRINGIFY(FLUID_INVERSE_MASS(environment))),
			util_shader_DEFINE("RADIUS", STRINGIFY(FLUID_TRIANGLE_RADIUS(environment))),
			util_shader_DEFINE("PARTICLE_INVERSE_MASS", STRINGIFY(FLUID_INVERSE_MASS(environment))),
			util_shader_DEFINE("BODY_MASSES_BINDING", STRINGIFY(game_logic__util_BODY_MASSES_BINDING)),
			//util_shader_DEFINE("BODY_INVERSE_MASS", STRINGIFY(INVERSE_MASS)),
			//util_shader_DEFINE("BODY_INVERSE_INERTIA", STRINGIFY(INVERSE_INERTIA)),
			util_shader_DEFINE("METER_INVERSE", STRINGIFY(game_logic__util__spatial_METER_INVERSE(environment))),
			util_shader_DEFINE("METER", STRINGIFY(game_logic__util__spatial_METER(environment))),
			util_shader_DEFINE("CAMERA_BINDING", STRINGIFY(game_CAMERA_BINDING)),
			game_PROJECTION_SCALE_DEFINITION(environment),
			::util::shader::file_to_string("util/fluid_triangle_contact_basis.vert")
		);
		::util::shader::set_shader_statically
		(
			fragment_shader,
			util_shader_VERSION,
			util_shader_DEFINE("COLOR", "vec4(0.0, 1.0, 1.0, 1.0)"),
			::util::shader::file_to_string("util/static_color.frag") // TODO: Should only be done once
		);
		environment.state.fluid_triangle_contact_basis_draw_shader = ::util::shader::create_program(vertex_shader, fragment_shader);
		std::cout << "Fluid triangle contact basis draw shader compiled" << std::endl;

		::util::shader::set_shader_statically
		(
			vertex_shader,
			util_shader_VERSION,
			util_shader_DEFINE("METER", STRINGIFY(game_logic__util__spatial_METER(environment))), // TODO: Remove
			util_shader_DEFINE("METER_INVERSE", STRINGIFY(game_logic__util__spatial_METER_INVERSE(environment))), // TODO: Remove
			util_shader_DEFINE("CONTACT_BINDING", STRINGIFY(game_logic__util_CONTACT_BINDING)),
			util_shader_DEFINE("CONTACT_SURFACE_BINDING", STRINGIFY(game_logic__util_CONTACT_SURFACE_BINDING)),
			max_contact_count_definition,
			max_rigid_body_count_definition,
			util_shader_DEFINE("POSITION_BINDING", STRINGIFY(game_logic__util_RIGID_BODY_POSITION_BINDING)),
			util_shader_DEFINE("CAMERA_BINDING", STRINGIFY(game_CAMERA_BINDING)),
			game_PROJECTION_SCALE_DEFINITION(environment),
			::util::shader::file_to_string("util/contact_basis.vert")
		);
		::util::shader::set_shader_statically
		(
			fragment_shader,
			util_shader_VERSION,
			::util::shader::file_to_string("util/contact_basis.frag")
		);
		environment.state.contact_basis_draw_shader = ::util::shader::create_program(vertex_shader, fragment_shader);
		std::cout << "Contact basis draw shader compiled" << std::endl;

		::util::shader::set_shader_statically
		(
			vertex_shader,
			util_shader_VERSION,
			util_shader_DEFINE("METER", STRINGIFY(game_logic__util__spatial_METER(environment))), // TODO: Remove
			util_shader_DEFINE("METER_INVERSE", STRINGIFY(game_logic__util__spatial_METER_INVERSE(environment))), // TODO: Remove
			util_shader_DEFINE("CONTACT_BINDING", STRINGIFY(game_logic__util_CONTACT_BINDING)),
			util_shader_DEFINE("CONTACT_SURFACE_BINDING", STRINGIFY(game_logic__util_CONTACT_SURFACE_BINDING)),
			max_contact_count_definition,
			max_rigid_body_count_definition,
			util_shader_DEFINE("POSITION_BINDING", STRINGIFY(game_logic__util_RIGID_BODY_POSITION_BINDING)),
			util_shader_DEFINE("CAMERA_BINDING", STRINGIFY(game_CAMERA_BINDING)),
			game_PROJECTION_SCALE_DEFINITION(environment),
			::util::shader::file_to_string("util/contact_impulses.vert")
		);
		::util::shader::set_shader_statically
		(
			fragment_shader,
			util_shader_VERSION,
			::util::shader::file_to_string("util/contact_impulses.frag")
		);
		environment.state.contact_impulses_draw_shader = ::util::shader::create_program(vertex_shader, fragment_shader);
		std::cout << "Contact impulses draw shader compiled" << std::endl;

		::util::shader::set_shader_statically
		(
			vertex_shader,
			util_shader_VERSION,
			util_shader_DEFINE("STEP", STRINGIFY(GRAVITY_SAMPLE_STEP(environment))), 
			util_shader_DEFINE("CAMERA_BINDING", STRINGIFY(game_CAMERA_BINDING))
			util_shader_DEFINE("RADIAN_INVERSE", STRINGIFY(game_logic__util__spatial_RADIAN_INVERSE(environment))),
			util_shader_DEFINE("METER", STRINGIFY(game_logic__util__spatial_METER(environment))), 
			util_shader_DEFINE("METER_INVERSE", STRINGIFY(game_logic__util__spatial_METER_INVERSE(environment))), 
			game_PROJECTION_SCALE_DEFINITION(environment),
			::util::shader::file_to_string("util/grid_points.vert")
		);
		::util::shader::set_shader_statically
		(
			fragment_shader,
			util_shader_VERSION,
			util_shader_DEFINE("COLOR", "vec4(1.0, 0.3, 1.0, 1.0)"),
			::util::shader::file_to_string("util/static_color.frag") // TODO: Should only be done once
		);
		environment.state.gravity_grid_points_draw_shader = ::util::shader::create_program(vertex_shader, fragment_shader);
		std::cout << "Gravity grid points draw shader compiled" << std::endl;

		::util::shader::set_shader_statically
		(
			vertex_shader,
			util_shader_VERSION,
			util_shader_DEFINE("STEP", STRINGIFY(GRAVITY_SAMPLE_STEP(environment))),
			util_shader_DEFINE("GRAVITY_SOURCES_BINDING", STRINGIFY(game_logic__util_GRAVITY_SOURCES_BINDING)),
			util_shader_DEFINE("MAX_GRAVITY_SOURCE_COUNT", STRINGIFY(MAX_GRAVITY_SOURCE_COUNT)),
			util_shader_DEFINE("CAMERA_BINDING", STRINGIFY(game_CAMERA_BINDING))
			util_shader_DEFINE("RADIAN_INVERSE", STRINGIFY(game_logic__util__spatial_RADIAN_INVERSE(environment))),
			util_shader_DEFINE("METER", STRINGIFY(game_logic__util__spatial_METER(environment))),
			util_shader_DEFINE("METER_INVERSE", STRINGIFY(game_logic__util__spatial_METER_INVERSE(environment))),
			game_PROJECTION_SCALE_DEFINITION(environment),
			::util::shader::file_to_string("util/gravity_directions.vert")
		);
		::util::shader::set_shader_statically
		(
			fragment_shader,
			util_shader_VERSION,
			util_shader_DEFINE("COLOR", "vec4(1.0, 0.3, 1.0, 1.0)"),
			::util::shader::file_to_string("util/static_color.frag") // TODO: Should only be done once
		);
		environment.state.gravity_directions_draw_shader = ::util::shader::create_program(vertex_shader, fragment_shader);
		std::cout << "Gravity directions draw shader compiled" << std::endl;

		::util::shader::set_shader_statically
		(
			vertex_shader,
			util_shader_VERSION,
			util_shader_DEFINE("GRAVITY_SOURCES_BINDING", STRINGIFY(game_logic__util_GRAVITY_SOURCES_BINDING)),
			util_shader_DEFINE("MAX_GRAVITY_SOURCE_COUNT", STRINGIFY(MAX_GRAVITY_SOURCE_COUNT)),
			util_shader_DEFINE("CAMERA_BINDING", STRINGIFY(game_CAMERA_BINDING))
			util_shader_DEFINE("RADIAN_INVERSE", STRINGIFY(game_logic__util__spatial_RADIAN_INVERSE(environment))),
			util_shader_DEFINE("METER", STRINGIFY(game_logic__util__spatial_METER(environment))),
			util_shader_DEFINE("METER_INVERSE", STRINGIFY(game_logic__util__spatial_METER_INVERSE(environment))),
			util_shader_DEFINE("GRAB_RADIUS", STRINGIFY(GRAVITY_SOURCE_GRAB_RADIUS(environment))),
			util_shader_DEFINE("LIGHT_DISTANCE", STRINGIFY(GRAVITY_SOURCE_LIGHT_DISTANCE(environment))),
			game_PROJECTION_SCALE_DEFINITION(environment),
			::util::shader::file_to_string("util/gravity_sources.vert")
		);
		::util::shader::set_shader_statically
		(
			fragment_shader,
			util_shader_VERSION,
			util_shader_DEFINE("GRAB_RADIUS", STRINGIFY(GRAVITY_SOURCE_GRAB_RADIUS(environment))),
			util_shader_DEFINE("LIGHT_DISTANCE", STRINGIFY(GRAVITY_SOURCE_LIGHT_DISTANCE(environment))),
			util_shader_DEFINE("RADIAN_INVERSE", STRINGIFY(game_logic__util__spatial_RADIAN_INVERSE(environment))),
			util_shader_DEFINE("METER", STRINGIFY(game_logic__util__spatial_METER(environment))),
			util_shader_DEFINE("METER_INVERSE", STRINGIFY(game_logic__util__spatial_METER_INVERSE(environment))),
			::util::shader::file_to_string("util/gravity_sources.frag") // TODO: Should only be done once
		);
		environment.state.gravity_sources_draw_shader = ::util::shader::create_program(vertex_shader, fragment_shader);
		std::cout << "Gravity sources draw shader compiled" << std::endl;

		::util::shader::set_shader_statically
		(
			vertex_shader,
			util_shader_VERSION,
			util_shader_DEFINE("GRAVITY_SOURCES_BINDING", STRINGIFY(game_logic__util_GRAVITY_SOURCES_BINDING)),
			util_shader_DEFINE("MAX_GRAVITY_SOURCE_COUNT", STRINGIFY(MAX_GRAVITY_SOURCE_COUNT)),
			util_shader_DEFINE("CAMERA_BINDING", STRINGIFY(game_CAMERA_BINDING))
			util_shader_DEFINE("RADIAN_INVERSE", STRINGIFY(game_logic__util__spatial_RADIAN_INVERSE(environment))),
			util_shader_DEFINE("METER", STRINGIFY(game_logic__util__spatial_METER(environment))),
			util_shader_DEFINE("METER_INVERSE", STRINGIFY(game_logic__util__spatial_METER_INVERSE(environment))),
			util_shader_DEFINE("GRAB_RADIUS", STRINGIFY(GRAVITY_SOURCE_GRAB_RADIUS(environment))),
			util_shader_DEFINE("LIGHT_DISTANCE", STRINGIFY(GRAVITY_SOURCE_LIGHT_DISTANCE(environment))),
			game_PROJECTION_SCALE_DEFINITION(environment),
			::util::shader::file_to_string("util/hovered_gravity_source_wireframe.vert")
		);
		::util::shader::set_shader_statically
		(
			fragment_shader,
			util_shader_VERSION,
			util_shader_DEFINE("GRAB_RADIUS", STRINGIFY(GRAVITY_SOURCE_GRAB_RADIUS(environment))),
			util_shader_DEFINE("LIGHT_DISTANCE", STRINGIFY(GRAVITY_SOURCE_LIGHT_DISTANCE(environment))),
			util_shader_DEFINE("RADIAN_INVERSE", STRINGIFY(game_logic__util__spatial_RADIAN_INVERSE(environment))),
			util_shader_DEFINE("METER", STRINGIFY(game_logic__util__spatial_METER(environment))),
			util_shader_DEFINE("METER_INVERSE", STRINGIFY(game_logic__util__spatial_METER_INVERSE(environment))),
			::util::shader::file_to_string("util/hovered_gravity_source_wireframe.frag") // TODO: Should only be done once
		);
		environment.state.hovered_gravity_source_wireframe_draw_shader = ::util::shader::create_program(vertex_shader, fragment_shader);
		environment.state.hovered_gravity_source_wireframe_draw_shader_hovered_gravity_source_uniform_location = glGetUniformLocation(environment.state.hovered_gravity_source_wireframe_draw_shader, "hovered_gravity_source");
		std::cout << "Hovered gravity source wireframe draw shader compiled. Hovered gravity source uniform location: " << environment.state.hovered_gravity_source_wireframe_draw_shader_hovered_gravity_source_uniform_location << std::endl;

		::util::shader::set_shader_statically
		(
			vertex_shader,
			util_shader_VERSION,
			util_shader_DEFINE("CURSOR_CONSTRAINED_POINT_BINDING", STRINGIFY(game_logic__util_CURSOR_CONSTRAINED_POINT_BINDING)),
			util_shader_DEFINE("POSITION_BINDING", STRINGIFY(game_logic__util_RIGID_BODY_POSITION_BINDING)),
			util_shader_DEFINE("MAX_RIGID_BODY_COUNT", STRINGIFY(game_logic_MAX_RIGID_BODY_COUNT(environment))),
			util_shader_DEFINE("CURSOR_POSITION_BINDING", STRINGIFY(game_logic__util_CURSOR_POSITION_BINDING)),
			util_shader_DEFINE("CAMERA_BINDING", STRINGIFY(game_CAMERA_BINDING)),
			util_shader_DEFINE("METER", STRINGIFY(game_logic__util__spatial_METER(environment))),
			util_shader_DEFINE("RADIAN_INVERSE", STRINGIFY(game_logic__util__spatial_RADIAN_INVERSE(environment))),
			game_PROJECTION_SCALE_DEFINITION(environment),
			::util::shader::file_to_string("util/cursor_constraint.vert")
		);
		::util::shader::set_shader_statically
		(
			fragment_shader,
			util_shader_VERSION,
			util_shader_DEFINE("COLOR", "vec4(1.0, 1.0, 0.0, 1.0)"),
			::util::shader::file_to_string("util/static_color.frag")
		);
		environment.state.cursor_constraint_draw_shader = ::util::shader::create_program(vertex_shader, fragment_shader);
		std::cout << "Cursor constraint draw shader compiled" << std::endl;

		::util::shader::set_shader_statically
		(
			vertex_shader,
			util_shader_VERSION,
			util_shader_DEFINE("DISTANCE_CONSTRAINT_BINDING", STRINGIFY(game_logic__util_DISTANCE_CONSTRAINT_BINDING)),
			max_distance_constraint_count_definition,
			util_shader_DEFINE("POSITION_BINDING", STRINGIFY(game_logic__util_RIGID_BODY_POSITION_BINDING)),
			util_shader_DEFINE("MAX_RIGID_BODY_COUNT", STRINGIFY(game_logic_MAX_RIGID_BODY_COUNT(environment))),
			util_shader_DEFINE("CURSOR_POSITION_BINDING", STRINGIFY(game_logic__util_CURSOR_POSITION_BINDING)),
			util_shader_DEFINE("CAMERA_BINDING", STRINGIFY(game_CAMERA_BINDING)),
			util_shader_DEFINE("METER", STRINGIFY(game_logic__util__spatial_METER(environment))),
			util_shader_DEFINE("RADIAN_INVERSE", STRINGIFY(game_logic__util__spatial_RADIAN_INVERSE(environment))),
			game_PROJECTION_SCALE_DEFINITION(environment),
			::util::shader::file_to_string("util/distance_constraints.vert")
		);
		::util::shader::set_shader_statically
		(
			fragment_shader,
			util_shader_VERSION,
			util_shader_DEFINE("COLOR", "vec4(0.0, 1.0, 0.0, 1.0)"),
			::util::shader::file_to_string("util/static_color.frag")
		);
		environment.state.distance_constraints_draw_shader = ::util::shader::create_program(vertex_shader, fragment_shader);
		std::cout << "Distance constraints draw shader compiled" << std::endl;

		::util::shader::set_shader_statically
		(
			vertex_shader,
			util_shader_VERSION,
			util_shader_DEFINE("DISTANCE_CONSTRAINT_BINDING", STRINGIFY(game_logic__util_DISTANCE_CONSTRAINT_BINDING)),
			max_distance_constraint_count_definition,
			util_shader_DEFINE("POSITION_BINDING", STRINGIFY(game_logic__util_RIGID_BODY_POSITION_BINDING)), 
			util_shader_DEFINE("MAX_RIGID_BODY_COUNT", STRINGIFY(game_logic_MAX_RIGID_BODY_COUNT(environment))),
			util_shader_DEFINE("CURSOR_POSITION_BINDING", STRINGIFY(game_logic__util_CURSOR_POSITION_BINDING)),
			util_shader_DEFINE("CAMERA_BINDING", STRINGIFY(game_CAMERA_BINDING)),
			util_shader_DEFINE("METER", STRINGIFY(game_logic__util__spatial_METER(environment))),
			util_shader_DEFINE("RADIAN_INVERSE", STRINGIFY(game_logic__util__spatial_RADIAN_INVERSE(environment))),
			game_PROJECTION_SCALE_DEFINITION(environment),
			::util::shader::file_to_string("util/distance_constraint_start.vert")
		);
		::util::shader::set_shader_statically
		(
			fragment_shader,
			util_shader_VERSION,
			util_shader_DEFINE("COLOR", "vec4(0.0, 1.0, 0.0, 1.0)"),
			::util::shader::file_to_string("util/static_color.frag")
		);
		environment.state.distance_constraint_start_draw_shader = ::util::shader::create_program(vertex_shader, fragment_shader);
		std::cout << "Distance constraint start draw shader compiled" << std::endl;

		::util::shader::set_shader_statically
		(
			vertex_shader,
			util_shader_VERSION,
			util_shader_DEFINE("CURSOR_POSITION_BINDING", STRINGIFY(game_logic__util_CURSOR_POSITION_BINDING)),
			util_shader_DEFINE("CAMERA_BINDING", STRINGIFY(game_CAMERA_BINDING)),
			game_PROJECTION_SCALE_DEFINITION(environment),
			::util::shader::file_to_string("util/cursor_position.vert")
		);
		::util::shader::set_shader_statically
		(
			fragment_shader,
			util_shader_VERSION,
			util_shader_DEFINE("COLOR", "vec4(1.0, 1.0, 0.0, 1.0)"),
			::util::shader::file_to_string("util/static_color.frag")
		);
		environment.state.cursor_position_draw_shader = ::util::shader::create_program(vertex_shader, fragment_shader);
		std::cout << "Cursor position draw shader compiled" << std::endl;

		::util::shader::delete_shader(vertex_shader);
		::util::shader::delete_shader(fragment_shader);

		GLuint const compute_shader{ ::util::shader::create_shader(GL_COMPUTE_SHADER) };
		{
			std::cout << '\n';

			std::chrono::time_point start_time = std::chrono::high_resolution_clock::now();
			::util::shader::set_shader_statically
			(
				compute_shader,
				util_shader_VERSION,
				::util::shader::file_to_string("util/test.comp")
			);
			environment.state.test_compute_shader = ::util::shader::create_program(compute_shader);
			std::chrono::time_point end_time = std::chrono::high_resolution_clock::now();
			std::cout << "Test compute shader compiled in " << 
				std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time) << 
				'\n' << std::endl;
		}

		::util::shader::set_shader_statically
		(
			compute_shader,
			util_shader_VERSION, 
			max_rigid_body_count_definition,
			util_shader_DEFINE("COUNT_BINDING", STRINGIFY(game_logic__util_COUNT_BINDING)),
			util_shader_DEFINE("POSITION_BINDING", STRINGIFY(game_logic__util_RIGID_BODY_POSITION_BINDING)),
			util_shader_DEFINE("VELOCITY_BINDING", STRINGIFY(game_logic__util_RIGID_BODY_VELOCITY_BINDING)),
			util_shader_DEFINE("LOCAL_SIZE", STRINGIFY(game_logic__util__rigid_body_VELOCITY_INTEGRATION_LOCAL_SIZE(environment))),
			util_shader_DEFINE("GRAVITY_SOURCES_BINDING", STRINGIFY(game_logic__util_GRAVITY_SOURCES_BINDING)),
			util_shader_DEFINE("MAX_GRAVITY_SOURCE_COUNT", STRINGIFY(MAX_GRAVITY_SOURCE_COUNT)),
			::util::shader::file_to_string("util/rigid_body_velocity_integration.comp")
		);
		environment.state.rigid_body_velocity_integration_shader = ::util::shader::create_program(compute_shader);
		std::cout << "Rigid body velocity integration shader compiled" << std::endl;

		// TODO: Think about how to deal with this temporary variable
		std::string fluid_padding_definition
		{
			"#define PADDING " + std::to_string(game_logic_FLUID_PARTICLE_BOUNDING_BOX_PADDING(environment)) + '\n'
		};
		std::string fluid_paritcle_physical_radius_definition
		{
			"#define RADIUS " + std::to_string(game_logic_FLUID_PARTICLE_PHYSICAL_RADIUS(environment)) + '\n'
		};
		::util::shader::set_shader_statically
		(
			compute_shader,
			util_shader_VERSION,
			max_fluid_particle_count_definition,
			util_shader_DEFINE("COUNT_BINDING", STRINGIFY(game_logic__util_COUNT_BINDING)),
			util_shader_DEFINE("FLUID_POSITION_BINDING", STRINGIFY(game_logic__util_FLUID_POSITION_BINDING)),
			util_shader_DEFINE("FLUID_VELOCITY_BINDING", STRINGIFY(game_logic__util_FLUID_VELOCITY_BINDING)),
			util_shader_DEFINE("FLUID_BOUNDING_BOX_BINDING", STRINGIFY(game_logic__util_FLUID_BOUNDING_BOX_BINDING)),
			util_shader_DEFINE("CHANGED_FLUID_BOUNDING_BOX_BINDING", STRINGIFY(game_logic__util_CHANGED_FLUID_BOUNDING_BOX_BINDING)),
			util_shader_DEFINE("LOCAL_SIZE", STRINGIFY(INTEGRATE_FLUID_VELOCITY_LOCAL_SIZE(environment))),
			util_shader_DEFINE("GRAVITY_SOURCES_BINDING", STRINGIFY(game_logic__util_GRAVITY_SOURCES_BINDING)), 
			util_shader_DEFINE("MAX_GRAVITY_SOURCE_COUNT", STRINGIFY(MAX_GRAVITY_SOURCE_COUNT)),
			fluid_padding_definition,
			fluid_paritcle_physical_radius_definition,
			::util::shader::file_to_string("util/integrate_fluid_velocity.comp")
		);
		environment.state.integrate_fluid_velocity_shader = ::util::shader::create_program(compute_shader);
		std::cout << "Integrate fluid velocity shader compiled" << std::endl;

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
			max_triangle_count_definition,
			max_vertex_count_definition,
			max_rigid_body_count_definition,
			util_shader_DEFINE("COUNT_BINDING", STRINGIFY(game_logic__util_COUNT_BINDING)),
			util_shader_DEFINE("POSITION_BINDING", STRINGIFY(game_logic__util_RIGID_BODY_POSITION_BINDING)),
			util_shader_DEFINE("TRIANGLE_BINDING", STRINGIFY(game_logic__util_TRIANGLE_BINDING)),
			util_shader_DEFINE("VERTEX_BINDING", STRINGIFY(game_logic__util_VERTEX_BINDING)),
			util_shader_DEFINE("BOUNDING_BOX_BINDING", STRINGIFY(game_logic__util_TRIANGLE_BOUNDING_BOX_BINDING)),
			util_shader_DEFINE("CHANGED_BOUNDING_BOX_BINDING", STRINGIFY(game_logic__util_TRIANGLE_CHANGED_BOUNDING_BOX_BINDING)),
			util_shader_DEFINE("VELOCITY_BINDING", STRINGIFY(game_logic__util_RIGID_BODY_VELOCITY_BINDING)),
			util_shader_DEFINE("LOCAL_SIZE", STRINGIFY(game_logic__util__rigid_body_TRIANGLE_BOUNDING_BOX_UPDATE_LOCAL_SIZE(environment))),
			util_shader_DEFINE("RADIAN_INVERSE", STRINGIFY(game_logic__util__spatial_RADIAN_INVERSE(environment))),
			util_shader_DEFINE("TRIANGLE_LEAFS_BASE_INDEX", STRINGIFY(game_logic_TRIANGLE_LEAFS_BASE_INDEX(environment))),
			::util::shader::file_to_string("util/triangle_bounding_box_update.comp")
		);
		environment.state.triangle_bounding_box_update_shader = ::util::shader::create_program(compute_shader);
		std::cout << "Triangle bounding box update shader compiled" << std::endl;

		::util::shader::set_shader_statically
		(
			compute_shader,
			util_shader_VERSION,
			max_distance_constraint_count_definition,
			max_rigid_body_count_definition,
			util_shader_DEFINE("POSITION_BINDING", STRINGIFY(game_logic__util_RIGID_BODY_POSITION_BINDING)),
			util_shader_DEFINE("DISTANCE_CONSTRAINT_BINDING", STRINGIFY(game_logic__util_DISTANCE_CONSTRAINT_BINDING)),
			util_shader_DEFINE("RADIAN_INVERSE", STRINGIFY(game_logic__util__spatial_RADIAN_INVERSE(environment))),
			util_shader_DEFINE("BODY_MASSES_BINDING", STRINGIFY(game_logic__util_BODY_MASSES_BINDING)),
			//util_shader_DEFINE("INVERSE_MASS", STRINGIFY(INVERSE_MASS)),
			//util_shader_DEFINE("INVERSE_INERTIA", STRINGIFY(INVERSE_INERTIA)),
			util_shader_DEFINE("METER_INVERSE", STRINGIFY(game_logic__util__spatial_METER_INVERSE(environment))),
			util_shader_DEFINE("VELOCITY_BINDING", STRINGIFY(game_logic__util_RIGID_BODY_VELOCITY_BINDING)),
			util_shader_DEFINE("METER", STRINGIFY(game_logic__util__spatial_METER(environment))),
			util_shader_DEFINE("RADIAN", STRINGIFY(game_logic__util__spatial_RADIAN(environment))),
			util_shader_DEFINE("ALLOWED_PENETRATION", STRINGIFY(game_logic_ALLOWED_DISTANCE_CONSTRAINT_PENETRATION(environment))),
			util_shader_DEFINE("PENETRATION_VELOCITY_SCALE", STRINGIFY(game_logic_DISTANCE_CONSTRAINT_PENETRATION_VELOCITY_SCALE(environment))),
			::util::shader::file_to_string("util/add_distance_constraint.comp")
		);
		environment.state.add_distance_constraint_shader = ::util::shader::create_program(compute_shader);
		std::cout << "Add distance constraint shader compiled" << std::endl;

		std::string fluid_physical_radius_definition{ "#define RADIUS " + std::to_string(game_logic_FLUID_PARTICLE_PHYSICAL_RADIUS(environment)) + '\n'};

		::util::shader::set_shader_statically
		(
			compute_shader,
			util_shader_VERSION,
			util_shader_DEFINE("FLUID_CONTACT_COUNT_BINDING", STRINGIFY(game_logic__util_FLUID_CONTACT_COUNT_BINDING)),
			util_shader_DEFINE("FLUID_CONTACT_BINDING", STRINGIFY(game_logic__util_FLUID_CONTACT_BINDING)),
			max_fluid_contact_count_definition,
			max_fluid_particle_count_definition,
			util_shader_DEFINE("FLUID_POSITION_BINDING", STRINGIFY(game_logic__util_FLUID_POSITION_BINDING)),
			util_shader_DEFINE("FLUID_VELOCITY_BINDING", STRINGIFY(game_logic__util_FLUID_VELOCITY_BINDING)),
			util_shader_DEFINE("LOCAL_SIZE", STRINGIFY(PERSIST_FLUID_CONTACT_LOCAL_SIZE(environment))),
			util_shader_DEFINE("INVERSE_MASS", STRINGIFY(FLUID_INVERSE_MASS(environment))),
			util_shader_DEFINE("STRENGTH_RADIUS", STRINGIFY(FLUID_STRENGTH_RADIUS(environment))),
			fluid_physical_radius_definition,
			util_shader_DEFINE("MAX_STRENGTH", STRINGIFY(FLUID_MAX_STRENGTH(environment))),
			util_shader_DEFINE("TARGET_RADIUS", STRINGIFY(FLUID_TARGET_RADIUS(environment))),
			util_shader_DEFINE("TARGET_VELOCITY_SCALE", STRINGIFY(FLUID_TARGET_VELOCITY_SCALE(environment))),
			util_shader_DEFINE("METER_INVERSE", STRINGIFY(game_logic__util__spatial_METER_INVERSE(environment))),
			util_shader_DEFINE("METER", STRINGIFY(game_logic__util__spatial_METER(environment))),
			::util::shader::file_to_string("util/persist_fluid_contacts.comp")
		);
		environment.state.persist_fluid_contacts_shader = ::util::shader::create_program(compute_shader);
		std::cout << "Persist fluid contacts shader compiled" << std::endl;

		::util::shader::set_shader_statically
		(
			compute_shader,
			util_shader_VERSION,
			util_shader_DEFINE("FLUID_TRIANGLE_CONTACT_COUNT_BINDING", STRINGIFY(game_logic__util_FLUID_TRIANGLE_CONTACT_COUNT_BINDING)),
			util_shader_DEFINE("FLUID_TRIANGLE_CONTACT_BINDING", STRINGIFY(game_logic__util_FLUID_TRIANGLE_CONTACT_BINDING)),
			max_fluid_triangle_contact_count_definition,
			max_fluid_particle_count_definition,
			util_shader_DEFINE("POSITION_BINDING", STRINGIFY(game_logic__util_RIGID_BODY_POSITION_BINDING)),
			max_rigid_body_count_definition, 
			util_shader_DEFINE("TRIANGLE_BINDING", STRINGIFY(game_logic__util_TRIANGLE_BINDING)),
			max_triangle_count_definition, 
			util_shader_DEFINE("VERTEX_BINDING", STRINGIFY(game_logic__util_VERTEX_BINDING)),
			max_vertex_count_definition, 
			util_shader_DEFINE("FLUID_POSITION_BINDING", STRINGIFY(game_logic__util_FLUID_POSITION_BINDING)),
			util_shader_DEFINE("FLUID_VELOCITY_BINDING", STRINGIFY(game_logic__util_FLUID_VELOCITY_BINDING)),
			util_shader_DEFINE("VELOCITY_BINDING", STRINGIFY(game_logic__util_RIGID_BODY_VELOCITY_BINDING)),
			util_shader_DEFINE("LOCAL_SIZE", STRINGIFY(PERSIST_FLUID_TRIANGLE_CONTACT_LOCAL_SIZE(environment))),
			util_shader_DEFINE("RADIAN_INVERSE", STRINGIFY(game_logic__util__spatial_RADIAN_INVERSE(environment))),
			util_shader_DEFINE("PARTICLE_INVERSE_MASS", STRINGIFY(FLUID_INVERSE_MASS(environment))),
			util_shader_DEFINE("BODY_MASSES_BINDING", STRINGIFY(game_logic__util_BODY_MASSES_BINDING)),
			//util_shader_DEFINE("BODY_INVERSE_MASS", STRINGIFY(INVERSE_MASS)),
			//util_shader_DEFINE("BODY_INVERSE_INERTIA", STRINGIFY(INVERSE_INERTIA)),
			util_shader_DEFINE("METER_INVERSE", STRINGIFY(game_logic__util__spatial_METER_INVERSE(environment))),
			util_shader_DEFINE("METER", STRINGIFY(game_logic__util__spatial_METER(environment))),
			::util::shader::file_to_string("util/persist_fluid_triangle_contacts.comp")
		);
		environment.state.persist_fluid_triangle_contacts_shader = ::util::shader::create_program(compute_shader);
		std::cout << "Persist fluid triangle contacts shader compiled" << std::endl;

		::util::shader::set_shader_statically
		(
			compute_shader,
			util_shader_VERSION,
			util_shader_DEFINE("FLUID_TRIANGLE_CONTACT_COUNT_BINDING", STRINGIFY(game_logic__util_FLUID_TRIANGLE_CONTACT_COUNT_BINDING)),
			util_shader_DEFINE("FLUID_TRIANGLE_CONTACT_BINDING", STRINGIFY(game_logic__util_FLUID_TRIANGLE_CONTACT_BINDING)),
			max_fluid_triangle_contact_count_definition,
			max_fluid_particle_count_definition,
			util_shader_DEFINE("POSITION_BINDING", STRINGIFY(game_logic__util_RIGID_BODY_POSITION_BINDING)),
			max_rigid_body_count_definition,
			util_shader_DEFINE("TRIANGLE_BINDING", STRINGIFY(game_logic__util_TRIANGLE_BINDING)),
			max_triangle_count_definition,
			util_shader_DEFINE("VERTEX_BINDING", STRINGIFY(game_logic__util_VERTEX_BINDING)),
			max_vertex_count_definition,
			util_shader_DEFINE("FLUID_POSITION_BINDING", STRINGIFY(game_logic__util_FLUID_POSITION_BINDING)),
			util_shader_DEFINE("FLUID_VELOCITY_BINDING", STRINGIFY(game_logic__util_FLUID_VELOCITY_BINDING)),
			util_shader_DEFINE("VELOCITY_BINDING", STRINGIFY(game_logic__util_RIGID_BODY_VELOCITY_BINDING)),
			util_shader_DEFINE("LOCAL_SIZE", STRINGIFY(NEW_FLUID_TRIANGLE_CONTACT_LOCAL_SIZE(environment))),
			util_shader_DEFINE("RADIAN_INVERSE", STRINGIFY(game_logic__util__spatial_RADIAN_INVERSE(environment))),
			util_shader_DEFINE("PARTICLE_INVERSE_MASS", STRINGIFY(FLUID_INVERSE_MASS(environment))),
			util_shader_DEFINE("BODY_MASSES_BINDING", STRINGIFY(game_logic__util_BODY_MASSES_BINDING)),
			//util_shader_DEFINE("BODY_INVERSE_MASS", STRINGIFY(INVERSE_MASS)),
			//util_shader_DEFINE("BODY_INVERSE_INERTIA", STRINGIFY(INVERSE_INERTIA)),
			util_shader_DEFINE("METER_INVERSE", STRINGIFY(game_logic__util__spatial_METER_INVERSE(environment))),
			util_shader_DEFINE("METER", STRINGIFY(game_logic__util__spatial_METER(environment))),
			::util::shader::file_to_string("util/new_fluid_triangle_contacts.comp")
		);
		environment.state.new_fluid_triangle_contacts_shader = ::util::shader::create_program(compute_shader);
		environment.state.new_fluid_triangle_contacts_shader_persistent_count_uniform_location = glGetUniformLocation
		(
			environment.state.new_fluid_triangle_contacts_shader, "persistent_count"
		);
		std::cout << "New fluid triangle contacts shader compiled. Persistent count uniform location: " << environment.state.new_fluid_triangle_contacts_shader_persistent_count_uniform_location << std::endl;

		::util::shader::set_shader_statically
		(
			compute_shader,
			util_shader_VERSION,
			util_shader_DEFINE("CONTACT_BINDING", STRINGIFY(game_logic__util_CONTACT_BINDING)),
			max_contact_count_definition,
			max_triangle_count_definition,
			max_vertex_count_definition,
			max_rigid_body_count_definition,
			util_shader_DEFINE("POSITION_BINDING", STRINGIFY(game_logic__util_RIGID_BODY_POSITION_BINDING)),
			util_shader_DEFINE("TRIANGLE_BINDING", STRINGIFY(game_logic__util_TRIANGLE_BINDING)),
			util_shader_DEFINE("VERTEX_BINDING", STRINGIFY(game_logic__util_VERTEX_BINDING)),
			util_shader_DEFINE("CONTACT_SURFACE_BINDING", STRINGIFY(game_logic__util_CONTACT_SURFACE_BINDING)),
			util_shader_DEFINE("CONTACT_COUNT_BINDING", STRINGIFY(game_logic__util_CONTACT_COUNT_BINDING)),
			util_shader_DEFINE("LOCAL_SIZE", STRINGIFY(game_logic__util__rigid_body_OLD_TRIANGLE_CONTACT_UPDATE_LOCAL_SIZE(environment))),
			util_shader_DEFINE("RADIAN_INVERSE", STRINGIFY(game_logic__util__spatial_RADIAN_INVERSE(environment))),
			util_shader_DEFINE("BODY_MASSES_BINDING", STRINGIFY(game_logic__util_BODY_MASSES_BINDING)),
			//util_shader_DEFINE("INVERSE_MASS", STRINGIFY(INVERSE_MASS)),
			//util_shader_DEFINE("INVERSE_INERTIA", STRINGIFY(INVERSE_INERTIA)),
			util_shader_DEFINE("METER_INVERSE", STRINGIFY(game_logic__util__spatial_METER_INVERSE(environment))),
			util_shader_DEFINE("VELOCITY_BINDING", STRINGIFY(game_logic__util_RIGID_BODY_VELOCITY_BINDING)),
			util_shader_DEFINE("METER", STRINGIFY(game_logic__util__spatial_METER(environment))),
			util_shader_DEFINE("RADIAN", STRINGIFY(game_logic__util__spatial_RADIAN(environment))),
			util_shader_DEFINE("POSITION_SNAPSHOT_BINDING", STRINGIFY(game_logic__util_POSITION_SNAPSHOT_BINDING)),
			util_shader_DEFINE("DIRECT_POSITION_CORRECTION", STRINGIFY(game_logic_DIRECT_POSITION_CORRECTION(environment))),
			util_shader_DEFINE("ALLOWED_PENETRATION", STRINGIFY(game_logic_ALLOWED_PENETRATION(environment))), 
			util_shader_DEFINE("PENETRATION_VELOCITY_SCALE", STRINGIFY(game_logic_PENETRATION_VELOCITY_SCALE(environment))),
			util_shader_DEFINE("POSITION_IMPULSE_SCALE", STRINGIFY(game_logic_POSITION_IMPULSE_SCALE(environment))),
			::util::shader::file_to_string("util/old_triangle_contact_update.comp")
		);
		environment.state.old_triangle_contact_update_shader = ::util::shader::create_program(compute_shader);
		std::cout << "Old triangle contact update shader compiled" << std::endl;

		::util::shader::set_shader_statically
		(
			compute_shader,
			util_shader_VERSION,
			util_shader_DEFINE("FLUID_CONTACT_COUNT_BINDING", STRINGIFY(game_logic__util_FLUID_CONTACT_COUNT_BINDING)),
			util_shader_DEFINE("FLUID_CONTACT_BINDING", STRINGIFY(game_logic__util_FLUID_CONTACT_BINDING)),
			max_fluid_contact_count_definition,
			max_fluid_particle_count_definition,
			util_shader_DEFINE("FLUID_POSITION_BINDING", STRINGIFY(game_logic__util_FLUID_POSITION_BINDING)),
			util_shader_DEFINE("FLUID_VELOCITY_BINDING", STRINGIFY(game_logic__util_FLUID_VELOCITY_BINDING)),
			util_shader_DEFINE("LOCAL_SIZE", STRINGIFY(NEW_FLUID_CONTACT_LOCAL_SIZE(environment))),
			util_shader_DEFINE("INVERSE_MASS", STRINGIFY(FLUID_INVERSE_MASS(environment))),
			util_shader_DEFINE("STRENGTH_RADIUS", STRINGIFY(FLUID_STRENGTH_RADIUS(environment))),
			util_shader_DEFINE("MAX_STRENGTH", STRINGIFY(FLUID_MAX_STRENGTH(environment))),
			util_shader_DEFINE("TARGET_RADIUS", STRINGIFY(FLUID_TARGET_RADIUS(environment))),
			fluid_physical_radius_definition,
			util_shader_DEFINE("TARGET_VELOCITY_SCALE", STRINGIFY(FLUID_TARGET_VELOCITY_SCALE(environment))),
			util_shader_DEFINE("METER_INVERSE", STRINGIFY(game_logic__util__spatial_METER_INVERSE(environment))),
			util_shader_DEFINE("METER", STRINGIFY(game_logic__util__spatial_METER(environment))),
			::util::shader::file_to_string("util/new_fluid_contacts.comp")
		);
		environment.state.new_fluid_contacts_shader = ::util::shader::create_program(compute_shader);
		environment.state.new_fluid_contacts_shader_persistent_count_uniform_location = glGetUniformLocation(environment.state.new_fluid_contacts_shader, "persistent_count");
		std::cout << "New fluid contacts shader compiled. Persistent count uniform location: " << environment.state.new_fluid_contacts_shader_persistent_count_uniform_location << std::endl;

		::util::shader::set_shader_statically
		(
			compute_shader,
			util_shader_VERSION,
			util_shader_DEFINE("CONTACT_BINDING", STRINGIFY(game_logic__util_CONTACT_BINDING)),
			max_contact_count_definition,
			max_triangle_count_definition,
			max_vertex_count_definition,
			max_rigid_body_count_definition,
			util_shader_DEFINE("POSITION_BINDING", STRINGIFY(game_logic__util_RIGID_BODY_POSITION_BINDING)),
			util_shader_DEFINE("TRIANGLE_BINDING", STRINGIFY(game_logic__util_TRIANGLE_BINDING)),
			util_shader_DEFINE("VERTEX_BINDING", STRINGIFY(game_logic__util_VERTEX_BINDING)),
			util_shader_DEFINE("CONTACT_SURFACE_BINDING", STRINGIFY(game_logic__util_CONTACT_SURFACE_BINDING)),
			util_shader_DEFINE("CONTACT_COUNT_BINDING", STRINGIFY(game_logic__util_CONTACT_COUNT_BINDING)),
			util_shader_DEFINE("PERSISTENT_CONTACT_COUNT_BINDING", STRINGIFY(game_logic__util_PERSISTENT_CONTACT_COUNT_BINDING)),
			util_shader_DEFINE("LOCAL_SIZE", STRINGIFY(game_logic__util__rigid_body_NEW_TRIANGLE_CONTACT_LOCAL_SIZE(environment))),
			util_shader_DEFINE("RADIAN_INVERSE", STRINGIFY(game_logic__util__spatial_RADIAN_INVERSE(environment))),
			util_shader_DEFINE("BODY_MASSES_BINDING", STRINGIFY(game_logic__util_BODY_MASSES_BINDING)),
			//util_shader_DEFINE("INVERSE_MASS", STRINGIFY(INVERSE_MASS)), 
			//util_shader_DEFINE("INVERSE_INERTIA", STRINGIFY(INVERSE_INERTIA)),
			util_shader_DEFINE("METER_INVERSE", STRINGIFY(game_logic__util__spatial_METER_INVERSE(environment))),
			util_shader_DEFINE("VELOCITY_BINDING", STRINGIFY(game_logic__util_RIGID_BODY_VELOCITY_BINDING)),
			util_shader_DEFINE("METER", STRINGIFY(game_logic__util__spatial_METER(environment))),
			util_shader_DEFINE("RADIAN", STRINGIFY(game_logic__util__spatial_RADIAN(environment))),
			util_shader_DEFINE("POSITION_SNAPSHOT_BINDING", STRINGIFY(game_logic__util_POSITION_SNAPSHOT_BINDING)),
			util_shader_DEFINE("DIRECT_POSITION_CORRECTION", STRINGIFY(game_logic_DIRECT_POSITION_CORRECTION(environment))),
			util_shader_DEFINE("ALLOWED_PENETRATION", STRINGIFY(game_logic_ALLOWED_PENETRATION(environment))),
			util_shader_DEFINE("PENETRATION_VELOCITY_SCALE", STRINGIFY(game_logic_PENETRATION_VELOCITY_SCALE(environment))),
			util_shader_DEFINE("POSITION_IMPULSE_SCALE", STRINGIFY(game_logic_POSITION_IMPULSE_SCALE(environment))),
			::util::shader::file_to_string("util/new_triangle_contact.comp")
		);
		environment.state.new_triangle_contact_shader = ::util::shader::create_program(compute_shader);
		std::cout << "New triangle contact shader compiled" << std::endl;

		::util::shader::set_shader_statically
		(
			compute_shader,
			util_shader_VERSION,
			max_distance_constraint_count_definition,
			max_rigid_body_count_definition,
			util_shader_DEFINE("POSITION_BINDING", STRINGIFY(game_logic__util_RIGID_BODY_POSITION_BINDING)),
			util_shader_DEFINE("DISTANCE_CONSTRAINT_BINDING", STRINGIFY(game_logic__util_DISTANCE_CONSTRAINT_BINDING)),
			util_shader_DEFINE("LOCAL_SIZE", STRINGIFY(game_logic_UPDATE_DISTANCE_CONSTRAINTS_LOCAL_SIZE(environment))),
			util_shader_DEFINE("RADIAN_INVERSE", STRINGIFY(game_logic__util__spatial_RADIAN_INVERSE(environment))),
			util_shader_DEFINE("BODY_MASSES_BINDING", STRINGIFY(game_logic__util_BODY_MASSES_BINDING)),
			//util_shader_DEFINE("INVERSE_MASS", STRINGIFY(INVERSE_MASS)),
			//util_shader_DEFINE("INVERSE_INERTIA", STRINGIFY(INVERSE_INERTIA)),
			util_shader_DEFINE("METER_INVERSE", STRINGIFY(game_logic__util__spatial_METER_INVERSE(environment))),
			util_shader_DEFINE("VELOCITY_BINDING", STRINGIFY(game_logic__util_RIGID_BODY_VELOCITY_BINDING)),
			util_shader_DEFINE("METER", STRINGIFY(game_logic__util__spatial_METER(environment))),
			util_shader_DEFINE("RADIAN", STRINGIFY(game_logic__util__spatial_RADIAN(environment))),
			util_shader_DEFINE("ALLOWED_PENETRATION", STRINGIFY(game_logic_ALLOWED_DISTANCE_CONSTRAINT_PENETRATION(environment))),
			util_shader_DEFINE("PENETRATION_VELOCITY_SCALE", STRINGIFY(game_logic_DISTANCE_CONSTRAINT_PENETRATION_VELOCITY_SCALE(environment))),
			::util::shader::file_to_string("util/update_distance_constraints.comp")
		);
		environment.state.update_distance_constraints_shader = ::util::shader::create_program(compute_shader);
		std::cout << "Update distance constraints shader compiled" << std::endl;

		std::string force_distance_definition
		{
			"#define FORCE_DISTANCE " + std::to_string(game_logic_FLUID_PARTICLE_PHYSICAL_RADIUS(environment)) + '\n'
		};
		::util::shader::set_shader_statically
		(
			compute_shader,
			util_shader_VERSION,
			util_shader_DEFINE("FLUID_CONTACT_COUNT_BINDING", STRINGIFY(game_logic__util_FLUID_CONTACT_COUNT_BINDING)),
			util_shader_DEFINE("FLUID_CONTACT_BINDING", STRINGIFY(game_logic__util_FLUID_CONTACT_BINDING)),
			max_fluid_contact_count_definition,
			max_fluid_particle_count_definition,
			util_shader_DEFINE("FLUID_POSITION_BINDING", STRINGIFY(game_logic__util_FLUID_POSITION_BINDING)),
			util_shader_DEFINE("FLUID_VELOCITY_BINDING", STRINGIFY(game_logic__util_FLUID_VELOCITY_BINDING)),
			util_shader_DEFINE("LOCAL_SIZE", STRINGIFY(WARM_START_FLUID_CONTACTS_LOCAL_SIZE(environment))),
			util_shader_DEFINE("INVERSE_MASS", STRINGIFY(FLUID_INVERSE_MASS(environment))),
			util_shader_DEFINE("STRENGTH_RADIUS", STRINGIFY(FLUID_STRENGTH_RADIUS(environment))),
			util_shader_DEFINE("MAX_STRENGTH", STRINGIFY(FLUID_MAX_STRENGTH(environment))),
			util_shader_DEFINE("TARGET_RADIUS", STRINGIFY(FLUID_TARGET_RADIUS(environment))),
			force_distance_definition,
			util_shader_DEFINE("METER_INVERSE", STRINGIFY(game_logic__util__spatial_METER_INVERSE(environment))),
			util_shader_DEFINE("METER", STRINGIFY(game_logic__util__spatial_METER(environment))),
			::util::shader::file_to_string("util/warm_start_fluid_contacts.comp")
		);
		environment.state.warm_start_fluid_contacts_shader = ::util::shader::create_program(compute_shader);
		std::cout << "Warm start fluid contacts shader compiled" << std::endl;

		::util::shader::set_shader_statically
		(
			compute_shader,
			util_shader_VERSION,
			util_shader_DEFINE("FLUID_TRIANGLE_CONTACT_COUNT_BINDING", STRINGIFY(game_logic__util_FLUID_TRIANGLE_CONTACT_COUNT_BINDING)),
			util_shader_DEFINE("FLUID_TRIANGLE_CONTACT_BINDING", STRINGIFY(game_logic__util_FLUID_TRIANGLE_CONTACT_BINDING)),
			max_fluid_triangle_contact_count_definition,
			max_fluid_particle_count_definition,
			util_shader_DEFINE("POSITION_BINDING", STRINGIFY(game_logic__util_RIGID_BODY_POSITION_BINDING)),
			max_rigid_body_count_definition,
			util_shader_DEFINE("TRIANGLE_BINDING", STRINGIFY(game_logic__util_TRIANGLE_BINDING)),
			max_triangle_count_definition,
			util_shader_DEFINE("VERTEX_BINDING", STRINGIFY(game_logic__util_VERTEX_BINDING)),
			max_vertex_count_definition,
			util_shader_DEFINE("VELOCITY_BINDING", STRINGIFY(game_logic__util_RIGID_BODY_VELOCITY_BINDING)),
			util_shader_DEFINE("FLUID_POSITION_BINDING", STRINGIFY(game_logic__util_FLUID_POSITION_BINDING)),
			util_shader_DEFINE("FLUID_VELOCITY_BINDING", STRINGIFY(game_logic__util_FLUID_VELOCITY_BINDING)),
			util_shader_DEFINE("LOCAL_SIZE", STRINGIFY(NEW_FLUID_TRIANGLE_CONTACT_LOCAL_SIZE(environment))),
			util_shader_DEFINE("RADIAN", STRINGIFY(game_logic__util__spatial_RADIAN(environment))),
			util_shader_DEFINE("RADIAN_INVERSE", STRINGIFY(game_logic__util__spatial_RADIAN_INVERSE(environment))),
			util_shader_DEFINE("PARTICLE_INVERSE_MASS", STRINGIFY(FLUID_INVERSE_MASS(environment))),
			util_shader_DEFINE("BODY_MASSES_BINDING", STRINGIFY(game_logic__util_BODY_MASSES_BINDING)),
			//util_shader_DEFINE("BODY_INVERSE_MASS", STRINGIFY(INVERSE_MASS)),
			//util_shader_DEFINE("BODY_INVERSE_INERTIA", STRINGIFY(INVERSE_INERTIA)),
			util_shader_DEFINE("METER_INVERSE", STRINGIFY(game_logic__util__spatial_METER_INVERSE(environment))),
			util_shader_DEFINE("METER", STRINGIFY(game_logic__util__spatial_METER(environment))),
			::util::shader::file_to_string("util/warm_start_fluid_triangle_contacts.comp")
		);
		environment.state.warm_start_fluid_triangle_contacts_shader = ::util::shader::create_program(compute_shader);
		std::cout << "Warm start fluid triangle contacts shader compiled" << std::endl;

		::util::shader::set_shader_statically
		(
			compute_shader,
			util_shader_VERSION,
			max_contact_count_definition,
			max_rigid_body_count_definition,
			util_shader_DEFINE("CONTACT_SURFACE_BINDING", STRINGIFY(game_logic__util_CONTACT_SURFACE_BINDING)),
			util_shader_DEFINE("PERSISTENT_CONTACT_COUNT_BINDING", STRINGIFY(game_logic__util_PERSISTENT_CONTACT_COUNT_BINDING)),
			util_shader_DEFINE("VELOCITY_SNAPSHOT_BINDING", STRINGIFY(game_logic__util_VELOCITY_SNAPSHOT_BINDING)),
			util_shader_DEFINE("VELOCITY_BINDING", STRINGIFY(game_logic__util_RIGID_BODY_VELOCITY_BINDING)),
			util_shader_DEFINE("LOCAL_SIZE", STRINGIFY(game_logic__util__rigid_body_WARM_START_CONTACT_IMPULSES_LOCAL_SIZE(environment))),
			util_shader_DEFINE("METER_INVERSE", STRINGIFY(game_logic__util__spatial_METER_INVERSE(environment))),
			util_shader_DEFINE("RADIAN_INVERSE", STRINGIFY(game_logic__util__spatial_RADIAN_INVERSE(environment))),
			util_shader_DEFINE("METER", STRINGIFY(game_logic__util__spatial_METER(environment))),
			util_shader_DEFINE("RADIAN", STRINGIFY(game_logic__util__spatial_RADIAN(environment))),
			util_shader_DEFINE("BODY_MASSES_BINDING", STRINGIFY(game_logic__util_BODY_MASSES_BINDING)),
			//util_shader_DEFINE("INVERSE_MASS", STRINGIFY(INVERSE_MASS)),
			//util_shader_DEFINE("INVERSE_INERTIA", STRINGIFY(INVERSE_INERTIA)),
			util_shader_DEFINE("NORMAL_IMPULSE_SCALE", STRINGIFY(game_logic_NORMAL_IMPULSE_SCALE(environment))),
			util_shader_DEFINE("TANGENT_IMPULSE_SCALE", STRINGIFY(game_logic_TANGENT_IMPULSE_SCALE(environment))),
			util_shader_DEFINE("FRICTION_COEFFICIENT", STRINGIFY(FRICTION_COEFFICIENT)),
			::util::shader::file_to_string("util/warm_start_contact_impulses.comp")
		);
		environment.state.warm_start_contact_impulses_shader = ::util::shader::create_program(compute_shader);
		std::cout << "Warm start contact impulses shader compiled" << std::endl;

		::util::shader::set_shader_statically
		(
			compute_shader,
			util_shader_VERSION,
			max_distance_constraint_count_definition,
			max_rigid_body_count_definition,
			util_shader_DEFINE("POSITION_BINDING", STRINGIFY(game_logic__util_RIGID_BODY_POSITION_BINDING)),
			util_shader_DEFINE("DISTANCE_CONSTRAINT_BINDING", STRINGIFY(game_logic__util_DISTANCE_CONSTRAINT_BINDING)),
			util_shader_DEFINE("LOCAL_SIZE", STRINGIFY(game_logic_WARM_START_DISTANCE_CONSTRAINTS_LOCAL_SIZE(environment))),
			util_shader_DEFINE("RADIAN_INVERSE", STRINGIFY(game_logic__util__spatial_RADIAN_INVERSE(environment))),
			util_shader_DEFINE("BODY_MASSES_BINDING", STRINGIFY(game_logic__util_BODY_MASSES_BINDING)),
			//util_shader_DEFINE("INVERSE_MASS", STRINGIFY(INVERSE_MASS)),
			//util_shader_DEFINE("INVERSE_INERTIA", STRINGIFY(INVERSE_INERTIA)),
			util_shader_DEFINE("METER_INVERSE", STRINGIFY(game_logic__util__spatial_METER_INVERSE(environment))),
			util_shader_DEFINE("VELOCITY_BINDING", STRINGIFY(game_logic__util_RIGID_BODY_VELOCITY_BINDING)),
			util_shader_DEFINE("METER", STRINGIFY(game_logic__util__spatial_METER(environment))),
			util_shader_DEFINE("RADIAN", STRINGIFY(game_logic__util__spatial_RADIAN(environment))),
			util_shader_DEFINE("ALLOWED_PENETRATION", STRINGIFY(game_logic_ALLOWED_DISTANCE_CONSTRAINT_PENETRATION(environment))),
			util_shader_DEFINE("PENETRATION_VELOCITY_SCALE", STRINGIFY(game_logic_DISTANCE_CONSTRAINT_PENETRATION_VELOCITY_SCALE(environment))),
			::util::shader::file_to_string("util/warm_start_distance_constraints.comp")
		);
		environment.state.warm_start_distance_constraints_shader = ::util::shader::create_program(compute_shader);
		std::cout << "Warm start distance constraints shader compiled" << std::endl;

		::util::shader::set_shader_statically
		(
			compute_shader,
			util_shader_VERSION,
			max_rigid_body_count_definition,
			util_shader_DEFINE("CURSOR_CONSTRAINED_POINT_BINDING", STRINGIFY(game_logic__util_CURSOR_CONSTRAINED_POINT_BINDING)),
			util_shader_DEFINE("CURSOR_POSITION_BINDING", STRINGIFY(game_logic__util_CURSOR_POSITION_BINDING)),
			util_shader_DEFINE("CURSOR_CONSTRAINT_BINDING", STRINGIFY(game_logic__util_CURSOR_CONSTRAINT_BINDING)),
			util_shader_DEFINE("POSITION_BINDING", STRINGIFY(game_logic__util_RIGID_BODY_POSITION_BINDING)),
			util_shader_DEFINE("VELOCITY_BINDING", STRINGIFY(game_logic__util_RIGID_BODY_VELOCITY_BINDING)),
			util_shader_DEFINE("METER_INVERSE", STRINGIFY(game_logic__util__spatial_METER_INVERSE(environment))),
			util_shader_DEFINE("RADIAN_INVERSE", STRINGIFY(game_logic__util__spatial_RADIAN_INVERSE(environment))),
			util_shader_DEFINE("METER", STRINGIFY(game_logic__util__spatial_METER(environment))),
			util_shader_DEFINE("RADIAN", STRINGIFY(game_logic__util__spatial_RADIAN(environment))),
			util_shader_DEFINE("BODY_MASSES_BINDING", STRINGIFY(game_logic__util_BODY_MASSES_BINDING)),
			//util_shader_DEFINE("INVERSE_MASS", STRINGIFY(INVERSE_MASS)),
			//util_shader_DEFINE("INVERSE_INERTIA", STRINGIFY(INVERSE_INERTIA)),
			util_shader_DEFINE("VELOCITY_SCALE", STRINGIFY(game_logic_CURSOR_CONSTRAINT_VELOCITY_SCALE(environment))),
			util_shader_DEFINE("IMPULSE_SCALE", STRINGIFY(game_logic_CURSOR_CONSTRAINT_IMPULSE_SCALE(environment))),
			::util::shader::file_to_string("util/update_and_warm_start_cursor_constraint.comp")
		);
		environment.state.update_and_warm_start_cursor_constraint_shader = ::util::shader::create_program(compute_shader);
		std::cout << "Update and warm start cursor constraint shader compiled" << std::endl;

		::util::shader::set_shader_statically
		(
			compute_shader,
			util_shader_VERSION,
			util_shader_DEFINE("FLUID_CONTACT_COUNT_BINDING", STRINGIFY(game_logic__util_FLUID_CONTACT_COUNT_BINDING)),
			util_shader_DEFINE("FLUID_CONTACT_BINDING", STRINGIFY(game_logic__util_FLUID_CONTACT_BINDING)),
			max_fluid_contact_count_definition,
			max_fluid_particle_count_definition,
			util_shader_DEFINE("FLUID_POSITION_BINDING", STRINGIFY(game_logic__util_FLUID_POSITION_BINDING)),
			util_shader_DEFINE("FLUID_VELOCITY_SNAPSHOT_BINDING", STRINGIFY(game_logic__util_FLUID_VELOCITY_SNAPSHOT_BINDING)),
			util_shader_DEFINE("FLUID_VELOCITY_BINDING", STRINGIFY(game_logic__util_FLUID_VELOCITY_BINDING)),
			util_shader_DEFINE("LOCAL_SIZE", STRINGIFY(SOLVE_FLUID_CONTACTS_LOCAL_SIZE(environment))),
			util_shader_DEFINE("IMPULSE_SCALE", STRINGIFY(SOLVE_FLUID_CONTACTS_IMPULSE_SCALE(environment))),
			util_shader_DEFINE("INVERSE_MASS", STRINGIFY(FLUID_INVERSE_MASS(environment))),
			util_shader_DEFINE("STRENGTH_RADIUS", STRINGIFY(FLUID_STRENGTH_RADIUS(environment))),
			util_shader_DEFINE("MAX_STRENGTH", STRINGIFY(FLUID_MAX_STRENGTH(environment))),
			util_shader_DEFINE("TARGET_RADIUS", STRINGIFY(FLUID_TARGET_RADIUS(environment))),
			util_shader_DEFINE("METER_INVERSE", STRINGIFY(game_logic__util__spatial_METER_INVERSE(environment))),
			util_shader_DEFINE("METER", STRINGIFY(game_logic__util__spatial_METER(environment))),
			::util::shader::file_to_string("util/solve_fluid_contacts.comp")
		);
		environment.state.solve_fluid_contacts_shader = ::util::shader::create_program(compute_shader);
		std::cout << "Solve fluid contacts shader compiled" << std::endl;

		::util::shader::set_shader_statically
		(
			compute_shader,
			util_shader_VERSION,
			util_shader_DEFINE("FLUID_TRIANGLE_CONTACT_COUNT_BINDING", STRINGIFY(game_logic__util_FLUID_TRIANGLE_CONTACT_COUNT_BINDING)),
			util_shader_DEFINE("FLUID_TRIANGLE_CONTACT_BINDING", STRINGIFY(game_logic__util_FLUID_TRIANGLE_CONTACT_BINDING)),
			max_fluid_triangle_contact_count_definition,
			max_fluid_particle_count_definition,
			util_shader_DEFINE("POSITION_BINDING", STRINGIFY(game_logic__util_RIGID_BODY_POSITION_BINDING)),
			max_rigid_body_count_definition,
			util_shader_DEFINE("TRIANGLE_BINDING", STRINGIFY(game_logic__util_TRIANGLE_BINDING)),
			max_triangle_count_definition,
			util_shader_DEFINE("VERTEX_BINDING", STRINGIFY(game_logic__util_VERTEX_BINDING)),
			max_vertex_count_definition,
			util_shader_DEFINE("VELOCITY_BINDING", STRINGIFY(game_logic__util_RIGID_BODY_VELOCITY_BINDING)),
			util_shader_DEFINE("FLUID_POSITION_BINDING", STRINGIFY(game_logic__util_FLUID_POSITION_BINDING)),
			util_shader_DEFINE("FLUID_VELOCITY_SNAPSHOT_BINDING", STRINGIFY(game_logic__util_FLUID_VELOCITY_SNAPSHOT_BINDING)),
			util_shader_DEFINE("VELOCITY_SNAPSHOT_BINDING", STRINGIFY(game_logic__util_VELOCITY_SNAPSHOT_BINDING)),
			util_shader_DEFINE("FLUID_VELOCITY_BINDING", STRINGIFY(game_logic__util_FLUID_VELOCITY_BINDING)),
			util_shader_DEFINE("LOCAL_SIZE", STRINGIFY(NEW_FLUID_TRIANGLE_CONTACT_LOCAL_SIZE(environment))),
			util_shader_DEFINE("RADIAN", STRINGIFY(game_logic__util__spatial_RADIAN(environment))),
			util_shader_DEFINE("RADIAN_INVERSE", STRINGIFY(game_logic__util__spatial_RADIAN_INVERSE(environment))),
			util_shader_DEFINE("PARTICLE_INVERSE_MASS", STRINGIFY(FLUID_INVERSE_MASS(environment))),
			util_shader_DEFINE("BODY_MASSES_BINDING", STRINGIFY(game_logic__util_BODY_MASSES_BINDING)),
			//util_shader_DEFINE("BODY_INVERSE_MASS", STRINGIFY(INVERSE_MASS)),
			//util_shader_DEFINE("BODY_INVERSE_INERTIA", STRINGIFY(INVERSE_INERTIA)),
			util_shader_DEFINE("METER_INVERSE", STRINGIFY(game_logic__util__spatial_METER_INVERSE(environment))),
			util_shader_DEFINE("METER", STRINGIFY(game_logic__util__spatial_METER(environment))),
			util_shader_DEFINE("IMPULSE_SCALE", STRINGIFY(SOLVE_FLUID_TRIANGLE_CONTACTS_IMPULSE_SCALE(environment))),
			::util::shader::file_to_string("util/solve_fluid_triangle_contacts.comp")
		);
		environment.state.solve_fluid_triangle_contacts_shader = ::util::shader::create_program(compute_shader);
		std::cout << "Solve fluid triangle contacts shader compiled" << std::endl;

		::util::shader::set_shader_statically
		(
			compute_shader,
			util_shader_VERSION,
			max_contact_count_definition,
			max_rigid_body_count_definition,
			util_shader_DEFINE("CONTACT_SURFACE_BINDING", STRINGIFY(game_logic__util_CONTACT_SURFACE_BINDING)),
			util_shader_DEFINE("CONTACT_COUNT_BINDING", STRINGIFY(game_logic__util_CONTACT_COUNT_BINDING)),
			util_shader_DEFINE("VELOCITY_SNAPSHOT_BINDING", STRINGIFY(game_logic__util_VELOCITY_SNAPSHOT_BINDING)),
			util_shader_DEFINE("VELOCITY_BINDING", STRINGIFY(game_logic__util_RIGID_BODY_VELOCITY_BINDING)),
			util_shader_DEFINE("LOCAL_SIZE", STRINGIFY(game_logic__util__rigid_body_SOLVE_CONTACT_VELOCITIES_LOCAL_SIZE(environment))),
			util_shader_DEFINE("METER_INVERSE", STRINGIFY(game_logic__util__spatial_METER_INVERSE(environment))),
			util_shader_DEFINE("RADIAN_INVERSE", STRINGIFY(game_logic__util__spatial_RADIAN_INVERSE(environment))),
			util_shader_DEFINE("METER", STRINGIFY(game_logic__util__spatial_METER(environment))),
			util_shader_DEFINE("RADIAN", STRINGIFY(game_logic__util__spatial_RADIAN(environment))),
			util_shader_DEFINE("BODY_MASSES_BINDING", STRINGIFY(game_logic__util_BODY_MASSES_BINDING)),
			//util_shader_DEFINE("INVERSE_MASS", STRINGIFY(INVERSE_MASS)),
			//util_shader_DEFINE("INVERSE_INERTIA", STRINGIFY(INVERSE_INERTIA)),
			util_shader_DEFINE("NORMAL_IMPULSE_SCALE", STRINGIFY(game_logic_NORMAL_IMPULSE_SCALE(environment))),
			util_shader_DEFINE("TANGENT_IMPULSE_SCALE", STRINGIFY(game_logic_TANGENT_IMPULSE_SCALE(environment))),
			util_shader_DEFINE("FRICTION_COEFFICIENT", STRINGIFY(FRICTION_COEFFICIENT)),
			::util::shader::file_to_string("util/solve_contact_velocities.comp")
		);
		environment.state.solve_contact_velocities_shader = ::util::shader::create_program(compute_shader);
		std::cout << "Solve contact velocities shader compiled" << std::endl;

		::util::shader::set_shader_statically
		(
			compute_shader,
			util_shader_VERSION,
			max_distance_constraint_count_definition,
			max_rigid_body_count_definition,
			util_shader_DEFINE("POSITION_BINDING", STRINGIFY(game_logic__util_RIGID_BODY_POSITION_BINDING)),
			util_shader_DEFINE("DISTANCE_CONSTRAINT_BINDING", STRINGIFY(game_logic__util_DISTANCE_CONSTRAINT_BINDING)),
			util_shader_DEFINE("LOCAL_SIZE", STRINGIFY(game_logic_SOLVE_DISTANCE_CONSTRAINTS_LOCAL_SIZE(environment))),
			util_shader_DEFINE("RADIAN_INVERSE", STRINGIFY(game_logic__util__spatial_RADIAN_INVERSE(environment))),
			util_shader_DEFINE("BODY_MASSES_BINDING", STRINGIFY(game_logic__util_BODY_MASSES_BINDING)),
			//util_shader_DEFINE("INVERSE_MASS", STRINGIFY(INVERSE_MASS)),
			util_shader_DEFINE("INVERSE_METER", STRINGIFY(game_logic__util__spatial_METER_INVERSE(environment))),
			util_shader_DEFINE("INVERSE_RADIAN", STRINGIFY(game_logic__util__spatial_RADIAN_INVERSE(environment))),
			//util_shader_DEFINE("INVERSE_INERTIA", STRINGIFY(INVERSE_INERTIA)),
			util_shader_DEFINE("METER_INVERSE", STRINGIFY(game_logic__util__spatial_METER_INVERSE(environment))),
			util_shader_DEFINE("VELOCITY_BINDING", STRINGIFY(game_logic__util_RIGID_BODY_VELOCITY_BINDING)),
			util_shader_DEFINE("METER", STRINGIFY(game_logic__util__spatial_METER(environment))),
			util_shader_DEFINE("RADIAN", STRINGIFY(game_logic__util__spatial_RADIAN(environment))),
			util_shader_DEFINE("ALLOWED_PENETRATION", STRINGIFY(game_logic_ALLOWED_DISTANCE_CONSTRAINT_PENETRATION(environment))),
			util_shader_DEFINE("PENETRATION_VELOCITY_SCALE", STRINGIFY(game_logic_DISTANCE_CONSTRAINT_PENETRATION_VELOCITY_SCALE(environment))),
			util_shader_DEFINE("IMPULSE_SCALE", STRINGIFY(game_logic_DISTANCE_CONSTRAINT_IMPULSE_SCALE(environment))),
			util_shader_DEFINE("VELOCITY_SNAPSHOT_BINDING", STRINGIFY(game_logic__util_VELOCITY_SNAPSHOT_BINDING)),
			::util::shader::file_to_string("util/solve_distance_constraints.comp")
		);
		environment.state.solve_distance_constraints_shader = ::util::shader::create_program(compute_shader);
		std::cout << "Solve distance constraints shader compiled" << std::endl;

		::util::shader::set_shader_statically
		(
			compute_shader,
			util_shader_VERSION,
			max_rigid_body_count_definition,
			util_shader_DEFINE("CURSOR_CONSTRAINED_POINT_BINDING", STRINGIFY(game_logic__util_CURSOR_CONSTRAINED_POINT_BINDING)),
			util_shader_DEFINE("CURSOR_POSITION_BINDING", STRINGIFY(game_logic__util_CURSOR_POSITION_BINDING)),
			util_shader_DEFINE("CURSOR_CONSTRAINT_BINDING", STRINGIFY(game_logic__util_CURSOR_CONSTRAINT_BINDING)),
			util_shader_DEFINE("POSITION_BINDING", STRINGIFY(game_logic__util_RIGID_BODY_POSITION_BINDING)),
			util_shader_DEFINE("VELOCITY_SNAPSHOT_BINDING", STRINGIFY(game_logic__util_VELOCITY_SNAPSHOT_BINDING)),
			util_shader_DEFINE("VELOCITY_BINDING", STRINGIFY(game_logic__util_RIGID_BODY_VELOCITY_BINDING)),
			util_shader_DEFINE("METER_INVERSE", STRINGIFY(game_logic__util__spatial_METER_INVERSE(environment))),
			util_shader_DEFINE("RADIAN_INVERSE", STRINGIFY(game_logic__util__spatial_RADIAN_INVERSE(environment))),
			util_shader_DEFINE("METER", STRINGIFY(game_logic__util__spatial_METER(environment))),
			util_shader_DEFINE("RADIAN", STRINGIFY(game_logic__util__spatial_RADIAN(environment))),
			util_shader_DEFINE("BODY_MASSES_BINDING", STRINGIFY(game_logic__util_BODY_MASSES_BINDING)),
			//util_shader_DEFINE("INVERSE_MASS", STRINGIFY(INVERSE_MASS)),
			//util_shader_DEFINE("INVERSE_INERTIA", STRINGIFY(INVERSE_INERTIA)),
			util_shader_DEFINE("VELOCITY_SCALE", STRINGIFY(game_logic_CURSOR_CONSTRAINT_VELOCITY_SCALE(environment))),
			util_shader_DEFINE("IMPULSE_SCALE", STRINGIFY(game_logic_CURSOR_CONSTRAINT_IMPULSE_SCALE(environment))),
			util_shader_DEFINE("MAX_IMPULSE", STRINGIFY(game_logic_CURSOR_CONSTRAINT_MAX_IMPULSE(environment))),
			::util::shader::file_to_string("util/solve_cursor_constraint.comp")
		);
		environment.state.solve_cursor_constraint_shader = ::util::shader::create_program(compute_shader);
		std::cout << "Solve cursor constraint shader compiled" << std::endl;

		::util::shader::delete_shader(compute_shader);

		std::cout << std::endl;

		// TODO: Consider putting buffers next to each other in game state
		// TODO: Probably flip position of position and velocity buffer
		GLuint buffers[]
		{ 
			environment.state.camera_buffer, 

			environment.state.GPU_buffers.fluid.positions.buffer,
			environment.state.GPU_buffers.fluid.velocities.buffer,
			environment.state.GPU_buffers.fluid.bounding_boxes.buffer,
			environment.state.GPU_buffers.fluid.changed_bounding_boxes.buffer,
			environment.state.GPU_buffers.fluid.contacts.buffer,
			environment.state.GPU_buffers.fluid.contact_count.buffer,
			environment.state.GPU_buffers.fluid.velocities.snapshot_buffer,

			environment.state.GPU_buffers.rigid_bodies.positions.buffer, 
			environment.state.GPU_buffers.rigid_bodies.velocities.buffer,
			environment.state.GPU_buffers.rigid_bodies.triangles.buffer,
			environment.state.GPU_buffers.rigid_bodies.triangles.vertices.buffer,
			environment.state.GPU_buffers.rigid_bodies.triangles.bounding_boxes.buffer, 
			environment.state.GPU_buffers.rigid_bodies.triangles.changed_bounding_boxes.buffer, 
			environment.state.contact_buffer,
			environment.state.contact_surface_buffer, 
			environment.state.GPU_buffers.rigid_bodies.triangles.contact_count.buffer, 
			environment.state.GPU_buffers.rigid_bodies.triangles.persistent_contact_count.buffer, 
			environment.state.GPU_buffers.rigid_bodies.velocities.snapshot_buffer, 
			environment.state.GPU_buffers.rigid_bodies.positions.snapshot_buffer, 
			environment.state.cursor_position_buffer, 
			environment.state.cursor_constrained_point_buffer, 
			environment.state.cursor_constraint_buffer, 
			environment.state.GPU_buffers.rigid_bodies.distance_constraints.buffer, 
			environment.state.GPU_buffers.fluid_triangle.contacts.buffer,
			environment.state.GPU_buffers.fluid_triangle.contact_count.buffer,
			environment.state.gravity_sources_buffer, 
			environment.state.count_buffer, 
			environment.state.body_masses_buffer
		};
		glCreateBuffers(std::size(buffers), buffers);
		environment.state.camera_buffer = buffers[0u];

		environment.state.GPU_buffers.fluid.positions.buffer = buffers[1u];
		environment.state.GPU_buffers.fluid.velocities.buffer = buffers[2u];
		environment.state.GPU_buffers.fluid.bounding_boxes.buffer = buffers[3u];
		environment.state.GPU_buffers.fluid.changed_bounding_boxes.buffer = buffers[4u];
		environment.state.GPU_buffers.fluid.contacts.buffer = buffers[5u];
		environment.state.GPU_buffers.fluid.contact_count.buffer = buffers[6u];
		environment.state.GPU_buffers.fluid.velocities.snapshot_buffer = buffers[7u];

		environment.state.GPU_buffers.rigid_bodies.positions.buffer = buffers[8u];
		environment.state.GPU_buffers.rigid_bodies.velocities.buffer = buffers[9u];
		environment.state.GPU_buffers.rigid_bodies.triangles.buffer = buffers[10u];
		environment.state.GPU_buffers.rigid_bodies.triangles.vertices.buffer = buffers[11u];
		environment.state.GPU_buffers.rigid_bodies.triangles.bounding_boxes.buffer = buffers[12u];
		environment.state.GPU_buffers.rigid_bodies.triangles.changed_bounding_boxes.buffer = buffers[13u];
		environment.state.contact_buffer = buffers[14u];
		environment.state.contact_surface_buffer = buffers[15u];
		environment.state.GPU_buffers.rigid_bodies.triangles.contact_count.buffer = buffers[16u];
		environment.state.GPU_buffers.rigid_bodies.triangles.persistent_contact_count.buffer = buffers[17u];
		environment.state.GPU_buffers.rigid_bodies.velocities.snapshot_buffer = buffers[18u];
		environment.state.GPU_buffers.rigid_bodies.positions.snapshot_buffer = buffers[19u];
		environment.state.cursor_position_buffer = buffers[20u];
		environment.state.cursor_constrained_point_buffer = buffers[21u];
		environment.state.cursor_constraint_buffer = buffers[22u];
		environment.state.GPU_buffers.rigid_bodies.distance_constraints.buffer = buffers[23u];
		environment.state.GPU_buffers.fluid_triangle.contacts.buffer = buffers[24u];
		environment.state.GPU_buffers.fluid_triangle.contact_count.buffer = buffers[25u];
		environment.state.gravity_sources_buffer = buffers[26u];
		environment.state.count_buffer = buffers[27u];
		environment.state.body_masses_buffer = buffers[28u];

		{ // Camera buffer
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

		environment.state.current_rigid_body_count = 0u * 80u * game_logic__util__rigid_body_TRIANGLE_BOUNDING_BOX_UPDATE_LOCAL_SIZE(environment);//500000u;
		environment.state.GPU_buffers.rigid_bodies.triangles.current_count = 1u * environment.state.current_rigid_body_count;
		environment.state.current_triangle_contact_count = 0u;
		environment.state.current_persistent_contact_count = 0u;
		environment.state.current_distance_constraint_count = 0u;
		environment.state.current_fluid_particle_count = 20u * INTEGRATE_FLUID_VELOCITY_LOCAL_SIZE(environment);
		environment.state.GPU_buffers.fluid.contact_count.current_contact_count = 0u;
		environment.state.GPU_buffers.fluid.contact_count.current_persistent_contact_count = 0u;
		environment.state.GPU_buffers.fluid_triangle.contact_count.current_contact_count = 0u;
		environment.state.GPU_buffers.fluid_triangle.contact_count.current_persistent_contact_count = 0u;
		environment.state.current_gravity_source_count = 0u;

		{ // Fluid position buffer
			GLuint const p_index
			{
				glGetProgramResourceIndex(environment.state.integrate_fluid_velocity_shader, GL_BUFFER_VARIABLE, "Fluid_Position.p")
			};

			GLenum const prop_labels[]{ GL_OFFSET, GL_ARRAY_STRIDE };
			GLint props[std::size(prop_labels)];
			glGetProgramResourceiv
			(
				environment.state.integrate_fluid_velocity_shader, GL_BUFFER_VARIABLE, p_index,
				std::size(prop_labels), prop_labels, 2, nullptr, props
			);
			// TODO: Consider putting p_offset and p_stride contigously in game state
			environment.state.GPU_buffers.fluid.positions.p_offset = props[0u];
			environment.state.GPU_buffers.fluid.positions.p_stride = props[1u];

#if USE_DYNAMIC_SIZES == true
			environment.state.GPU_buffers.fluid.positions.size = environment.state.GPU_buffers.fluid.positions.p_offset + game_logic_MAX_FLUID_PARTICLE_COUNT(environment) * environment.state.GPU_buffers.fluid.positions.p_stride;
#else
			GLuint const block_index
			{
				glGetProgramResourceIndex(environment.state.integrate_fluid_velocity_shader, GL_SHADER_STORAGE_BLOCK, "Fluid_Position")
			};
			GLenum const buffer_size_label{ GL_BUFFER_DATA_SIZE };
			glGetProgramResourceiv
			(
				environment.state.integrate_fluid_velocity_shader, GL_SHADER_STORAGE_BLOCK, block_index,
				1, &buffer_size_label, 1, nullptr, &environment.state.fluid_position_buffer_size
			);
#endif

			// TODO: Don't initialize a few positions by copying over the ENTIRE buffer 
			// content from CPU to GPU like this. Instead, use persistent mapping 
			// for both initialization and updating.
			unsigned char* const initial_fluid_position = new unsigned char[environment.state.GPU_buffers.fluid.positions.size];

			for (GLuint i = 0; i < environment.state.current_fluid_particle_count; ++i)
			{
				GLuint const width{ 100u };

				::util::math::Vector_2D position
				{
					-game_logic__util__spatial_FROM_METERS(environment, (30.0f + 0.5f * (i % width) * cos(i * 0.1f))),
					-game_logic__util__spatial_FROM_METERS(environment, (30.0f + 0.5f * (i / width) * sin(i * 0.2f)))
				};

				std::memcpy
				(
					initial_fluid_position + environment.state.GPU_buffers.fluid.positions.p_offset + i * environment.state.GPU_buffers.fluid.positions.p_stride,
					&position, sizeof(position)
				);
			}

			glNamedBufferStorage
			(
				environment.state.GPU_buffers.fluid.positions.buffer, environment.state.GPU_buffers.fluid.positions.size, initial_fluid_position,
				0u//GL_MAP_PERSISTENT_BIT | GL_MAP_READ_BIT | GL_MAP_WRITE_BIT //0u
			);

			delete[] initial_fluid_position;

			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, game_logic__util_FLUID_POSITION_BINDING, environment.state.GPU_buffers.fluid.positions.buffer);

			/*environment.state.position_mapping = static_cast<unsigned char*>
			(
				glMapNamedBufferRange
				(
					environment.state.rigid_body_position_buffer,
					0u, environment.state.rigid_body_position_buffer_size,
					GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_FLUSH_EXPLICIT_BIT
				)
			);*/
		}

		{ // Fluid velocity buffer
			GLuint const v_index
			{
				glGetProgramResourceIndex(environment.state.integrate_fluid_velocity_shader, GL_BUFFER_VARIABLE, "Fluid_Velocity.v")
			};

			GLenum const prop_labels[]{ GL_OFFSET, GL_ARRAY_STRIDE };
			GLint props[std::size(prop_labels)];
			glGetProgramResourceiv
			(
				environment.state.integrate_fluid_velocity_shader, GL_BUFFER_VARIABLE, v_index,
				std::size(prop_labels), prop_labels, 2, nullptr, props
			);
			// TODO: Consider putting v_offset and v_stride contigously in game state
			environment.state.GPU_buffers.fluid.velocities.v_offset = props[0u];
			environment.state.GPU_buffers.fluid.velocities.v_stride = props[1u];

#if USE_DYNAMIC_SIZES == true
			environment.state.GPU_buffers.fluid.velocities.size = environment.state.GPU_buffers.fluid.velocities.v_offset + game_logic_MAX_FLUID_PARTICLE_COUNT(environment) * environment.state.GPU_buffers.fluid.velocities.v_stride;
#else
			GLuint const block_index
			{
				glGetProgramResourceIndex(environment.state.integrate_fluid_velocity_shader, GL_SHADER_STORAGE_BLOCK, "Fluid_Velocity")
			};
			GLenum const buffer_size_label{ GL_BUFFER_DATA_SIZE };
			glGetProgramResourceiv
			(
				environment.state.integrate_fluid_velocity_shader, GL_SHADER_STORAGE_BLOCK, block_index,
				1, &buffer_size_label, 1, nullptr, &environment.state.fluid_velocity_buffer_size
			);
#endif

			// TODO: Don't initialize a few velocities by copying over the ENTIRE buffer 
			// content from CPU to GPU like this. Instead, use persistent mapping 
			// for both initialization and updating.
			unsigned char* const initial_fluid_velocity = new unsigned char[environment.state.GPU_buffers.fluid.velocities.size];

			for (GLuint i = 0; i < environment.state.current_fluid_particle_count; ++i)
			{
				//GLuint const width{ 316u };

				::util::math::Vector_2D velocity
				{
					game_METERS_PER_SECOND_TO_LENGTH_PER_TICK(environment, 0.0f),
					game_METERS_PER_SECOND_TO_LENGTH_PER_TICK(environment, 0.0f)
				};
				if (i % 2u == 0u)
				{
					//velocity.x = game_METERS_PER_SECOND_TO_LENGTH_PER_TICK(environment, -10.0f);
				}

				std::memcpy
				(
					initial_fluid_velocity + environment.state.GPU_buffers.fluid.velocities.v_offset + i * environment.state.GPU_buffers.fluid.velocities.v_stride,
					&velocity, sizeof(velocity)
				);
			}

			glNamedBufferStorage
			(
				environment.state.GPU_buffers.fluid.velocities.buffer, environment.state.GPU_buffers.fluid.velocities.size, initial_fluid_velocity,
				0u//GL_MAP_PERSISTENT_BIT | GL_MAP_READ_BIT | GL_MAP_WRITE_BIT
			);

			delete[] initial_fluid_velocity;

			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, game_logic__util_FLUID_VELOCITY_BINDING, environment.state.GPU_buffers.fluid.velocities.buffer);

			/*environment.state.position_mapping = static_cast<unsigned char*>
			(
				glMapNamedBufferRange
				(
					environment.state.rigid_body_position_buffer,
					0u, environment.state.rigid_body_position_buffer_size,
					GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_FLUSH_EXPLICIT_BIT
				)
			);*/
		}

		{ // Fluid bounding box buffer
			GLuint const boxes_index
			{
				glGetProgramResourceIndex(environment.state.integrate_fluid_velocity_shader, GL_BUFFER_VARIABLE, "Fluid_Bounding_Boxes.boxes")
			};

			GLenum const prop_labels[]{ GL_OFFSET, GL_ARRAY_STRIDE };
			GLint props[std::size(prop_labels)];
			glGetProgramResourceiv
			(
				environment.state.integrate_fluid_velocity_shader, GL_BUFFER_VARIABLE, boxes_index,
				std::size(prop_labels), prop_labels, 2u, nullptr, props
			);
			// TODO: Consider putting offset and stride contigously in game state
			environment.state.GPU_buffers.fluid.bounding_boxes.boxes_offset = props[0u];
			environment.state.GPU_buffers.fluid.bounding_boxes.boxes_stride = props[1u];

#if USE_DYNAMIC_SIZES == true
			environment.state.GPU_buffers.fluid.bounding_boxes.size = environment.state.GPU_buffers.fluid.bounding_boxes.boxes_offset + game_logic_MAX_FLUID_PARTICLE_COUNT(environment) * environment.state.GPU_buffers.fluid.bounding_boxes.boxes_stride;
#else
			GLuint const block_index
			{
				glGetProgramResourceIndex(environment.state.integrate_fluid_velocity_shader, GL_SHADER_STORAGE_BLOCK, "Fluid_Bounding_Boxes")
			};
			GLenum const buffer_size_label{ GL_BUFFER_DATA_SIZE };
			glGetProgramResourceiv
			(
				environment.state.integrate_fluid_velocity_shader, GL_SHADER_STORAGE_BLOCK, block_index,
				1u, &buffer_size_label, 1u, nullptr, &environment.state.fluid_bounding_box_buffer_size
			);
#endif

			// TODO: Don't initialize a few boxes by copying over the ENTIRE buffer 
			// content from CPU to GPU like this. Instead, use persistent mapping 
			// for both initialization and updating.
			unsigned char* const initial_boxes = new unsigned char[environment.state.GPU_buffers.fluid.bounding_boxes.size];

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
			for (GLuint i = 0u; i < environment.state.current_fluid_particle_count; ++i)
			{
				std::memcpy
				(
					initial_boxes + environment.state.GPU_buffers.fluid.bounding_boxes.boxes_offset + i * environment.state.GPU_buffers.fluid.bounding_boxes.boxes_stride,
					&box, sizeof(box)
				);
			}

			glNamedBufferStorage
			(
				environment.state.GPU_buffers.fluid.bounding_boxes.buffer, environment.state.GPU_buffers.fluid.bounding_boxes.size, initial_boxes,
				0u
			);

			delete[] initial_boxes;

			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, game_logic__util_FLUID_BOUNDING_BOX_BINDING, environment.state.GPU_buffers.fluid.bounding_boxes.buffer);
		}

		{ // Changed fluid bounding box buffer
			{
				GLuint const size_index
				{
					glGetProgramResourceIndex(environment.state.integrate_fluid_velocity_shader, GL_BUFFER_VARIABLE, "Changed_Fluid_Bounding_Boxes.size")
				};
				GLenum const offset_label{ GL_OFFSET };
				glGetProgramResourceiv
				(
					environment.state.integrate_fluid_velocity_shader, GL_BUFFER_VARIABLE, size_index,
					1u, &offset_label, 1u, nullptr, &environment.state.GPU_buffers.fluid.changed_bounding_boxes.size_offset
				);
			}

			{
				GLuint const boxes_index_index
				{
					glGetProgramResourceIndex(environment.state.integrate_fluid_velocity_shader, GL_BUFFER_VARIABLE, "Changed_Fluid_Bounding_Boxes.boxes[0].index")
				};
				GLenum const prop_labels[]{ GL_OFFSET, GL_TOP_LEVEL_ARRAY_STRIDE };
				GLint props[std::size(prop_labels)];
				glGetProgramResourceiv
				(
					environment.state.integrate_fluid_velocity_shader, GL_BUFFER_VARIABLE, boxes_index_index,
					std::size(prop_labels), prop_labels, 2u, nullptr, props
				);
				// TODO: Consider putting offset and stride contigously in game state
				environment.state.GPU_buffers.fluid.changed_bounding_boxes.boxes_index_offset = props[0u];
				environment.state.GPU_buffers.fluid.changed_bounding_boxes.boxes_stride = props[1u];

				GLenum const offset_label{ GL_OFFSET };

				GLuint const boxes_min_x_index
				{
					glGetProgramResourceIndex(environment.state.integrate_fluid_velocity_shader, GL_BUFFER_VARIABLE, "Changed_Fluid_Bounding_Boxes.boxes[0].min_x")
				};
				glGetProgramResourceiv
				(
					environment.state.integrate_fluid_velocity_shader, GL_BUFFER_VARIABLE, boxes_min_x_index,
					1u, &offset_label, 1u, nullptr, &environment.state.GPU_buffers.fluid.changed_bounding_boxes.boxes_min_x_offset
				);

				GLuint const boxes_min_y_index
				{
					glGetProgramResourceIndex(environment.state.integrate_fluid_velocity_shader, GL_BUFFER_VARIABLE, "Changed_Fluid_Bounding_Boxes.boxes[0].min_y")
				};
				glGetProgramResourceiv
				(
					environment.state.integrate_fluid_velocity_shader, GL_BUFFER_VARIABLE, boxes_min_y_index,
					1u, &offset_label, 1u, nullptr, &environment.state.GPU_buffers.fluid.changed_bounding_boxes.boxes_min_y_offset
				);

				GLuint const boxes_max_x_index
				{
					glGetProgramResourceIndex(environment.state.integrate_fluid_velocity_shader, GL_BUFFER_VARIABLE, "Changed_Fluid_Bounding_Boxes.boxes[0].max_x")
				};
				glGetProgramResourceiv
				(
					environment.state.integrate_fluid_velocity_shader, GL_BUFFER_VARIABLE, boxes_max_x_index,
					1u, &offset_label, 1u, nullptr, &environment.state.GPU_buffers.fluid.changed_bounding_boxes.boxes_max_x_offset
				);

				GLuint const boxes_max_y_index
				{
					glGetProgramResourceIndex(environment.state.integrate_fluid_velocity_shader, GL_BUFFER_VARIABLE, "Changed_Fluid_Bounding_Boxes.boxes[0].max_y")
				};
				glGetProgramResourceiv
				(
					environment.state.integrate_fluid_velocity_shader, GL_BUFFER_VARIABLE, boxes_max_y_index,
					1u, &offset_label, 1u, nullptr, &environment.state.GPU_buffers.fluid.changed_bounding_boxes.boxes_max_y_offset
				);
			}

#if USE_DYNAMIC_SIZES == true
			GLint const offsets[]
			{
			   environment.state.GPU_buffers.fluid.changed_bounding_boxes.boxes_index_offset,
			   environment.state.GPU_buffers.fluid.changed_bounding_boxes.boxes_min_x_offset,
			   environment.state.GPU_buffers.fluid.changed_bounding_boxes.boxes_min_y_offset,
			   environment.state.GPU_buffers.fluid.changed_bounding_boxes.boxes_max_x_offset,
			   environment.state.GPU_buffers.fluid.changed_bounding_boxes.boxes_max_y_offset
			};
			GLint const offset{ *std::min_element(std::begin(offsets), std::end(offsets)) };
			environment.state.GPU_buffers.fluid.changed_bounding_boxes.size = offset + game_logic_MAX_FLUID_PARTICLE_COUNT(environment) * environment.state.GPU_buffers.fluid.changed_bounding_boxes.boxes_stride;
#else
			GLuint const block_index
			{
				glGetProgramResourceIndex(environment.state.integrate_fluid_velocity_shader, GL_SHADER_STORAGE_BLOCK, "Changed_Fluid_Bounding_Boxes")
			};
			GLenum const buffer_size_label{ GL_BUFFER_DATA_SIZE };
			glGetProgramResourceiv
			(
				environment.state.integrate_fluid_velocity_shader, GL_SHADER_STORAGE_BLOCK, block_index,
				1u, &buffer_size_label, 1u, nullptr, &environment.state.GPU_buffers.fluid.changed_bounding_boxes.size
			);
#endif

			glNamedBufferStorage
			(
				environment.state.GPU_buffers.fluid.changed_bounding_boxes.buffer, environment.state.GPU_buffers.fluid.changed_bounding_boxes.size, nullptr,
				GL_MAP_READ_BIT | GL_MAP_PERSISTENT_BIT
			);
			glClearNamedBufferSubData
			(
				environment.state.GPU_buffers.fluid.changed_bounding_boxes.buffer,
				GL_R32UI,
				environment.state.GPU_buffers.fluid.changed_bounding_boxes.size_offset, sizeof(GLuint),
				GL_RED_INTEGER, GL_UNSIGNED_INT,
				nullptr
			);

			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, game_logic__util_CHANGED_FLUID_BOUNDING_BOX_BINDING, environment.state.GPU_buffers.fluid.changed_bounding_boxes.buffer);

			environment.state.GPU_buffers.fluid.changed_bounding_boxes.mapping = static_cast<unsigned char*>
				(
					glMapNamedBufferRange
					(
						environment.state.GPU_buffers.fluid.changed_bounding_boxes.buffer,
						0u, environment.state.GPU_buffers.fluid.changed_bounding_boxes.size,
						GL_MAP_READ_BIT | GL_MAP_PERSISTENT_BIT
					)
					);
		}

		{ // Fluid contact buffer
			{
				GLuint const contacts_inactive_index
				{
					glGetProgramResourceIndex(environment.state.persist_fluid_contacts_shader, GL_BUFFER_VARIABLE, "Fluid_Contacts.contacts[0].inactive")
				};
				GLenum const prop_labels[]{ GL_OFFSET, GL_TOP_LEVEL_ARRAY_STRIDE };
				GLint props[std::size(prop_labels)];
				glGetProgramResourceiv
				(
					environment.state.persist_fluid_contacts_shader, GL_BUFFER_VARIABLE, contacts_inactive_index,
					std::size(prop_labels), prop_labels, 2u, nullptr, props
				);
				// TODO: Consider putting offset and stride contigously in game state
				environment.state.GPU_buffers.fluid.contacts.contacts_inactive_offset = props[0u];
				environment.state.GPU_buffers.fluid.contacts.contacts_stride = props[1u];

				GLenum const offset_label{ GL_OFFSET };

				GLuint const impulse_index
				{
					glGetProgramResourceIndex(environment.state.persist_fluid_contacts_shader, GL_BUFFER_VARIABLE, "Fluid_Contacts.contacts[0].impulse")
				};
				glGetProgramResourceiv
				(
					environment.state.persist_fluid_contacts_shader, GL_BUFFER_VARIABLE, impulse_index,
					1u, &offset_label, 1u, nullptr, &environment.state.GPU_buffers.fluid.contacts.contacts_impulse_offset
				);

				GLuint const contacts_particles_index
				{
					glGetProgramResourceIndex(environment.state.persist_fluid_contacts_shader, GL_BUFFER_VARIABLE, "Fluid_Contacts.contacts[0].particles")
				};
				glGetProgramResourceiv
				(
					environment.state.persist_fluid_contacts_shader, GL_BUFFER_VARIABLE, contacts_particles_index,
					1u, &offset_label, 1u, nullptr, &environment.state.GPU_buffers.fluid.contacts.contacts_particles_offset
				);

				GLuint const direction_index
				{
					glGetProgramResourceIndex(environment.state.persist_fluid_contacts_shader, GL_BUFFER_VARIABLE, "Fluid_Contacts.contacts[0].direction")
				};
				glGetProgramResourceiv
				(
					environment.state.persist_fluid_contacts_shader, GL_BUFFER_VARIABLE, direction_index,
					1u, &offset_label, 1u, nullptr, &environment.state.GPU_buffers.fluid.contacts.contacts_direction_offset
				);

				GLuint const target_velocity_index
				{
					glGetProgramResourceIndex(environment.state.persist_fluid_contacts_shader, GL_BUFFER_VARIABLE, "Fluid_Contacts.contacts[0].target_velocity")
				};
				glGetProgramResourceiv
				(
					environment.state.persist_fluid_contacts_shader, GL_BUFFER_VARIABLE, target_velocity_index,
					1u, &offset_label, 1u, nullptr, &environment.state.GPU_buffers.fluid.contacts.contacts_target_velocity_offset
				);

				GLuint const mass_index
				{
					glGetProgramResourceIndex(environment.state.persist_fluid_contacts_shader, GL_BUFFER_VARIABLE, "Fluid_Contacts.contacts[0].mass")
				};
				glGetProgramResourceiv
				(
					environment.state.persist_fluid_contacts_shader, GL_BUFFER_VARIABLE, mass_index,
					1u, &offset_label, 1u, nullptr, &environment.state.GPU_buffers.fluid.contacts.contacts_mass_offset
				);

				GLuint const impulse_range_index
				{
					glGetProgramResourceIndex(environment.state.persist_fluid_contacts_shader, GL_BUFFER_VARIABLE, "Fluid_Contacts.contacts[0].impulse_range")
				};
				glGetProgramResourceiv
				(
					environment.state.persist_fluid_contacts_shader, GL_BUFFER_VARIABLE, impulse_range_index,
					1u, &offset_label, 1u, nullptr, &environment.state.GPU_buffers.fluid.contacts.contacts_impulse_range_offset
				);
			}

			GLint const offsets[]
			{
			   environment.state.GPU_buffers.fluid.contacts.contacts_inactive_offset,
			   environment.state.GPU_buffers.fluid.contacts.contacts_impulse_offset,
			   environment.state.GPU_buffers.fluid.contacts.contacts_particles_offset,
			   environment.state.GPU_buffers.fluid.contacts.contacts_direction_offset,
			   environment.state.GPU_buffers.fluid.contacts.contacts_target_velocity_offset,
			   environment.state.GPU_buffers.fluid.contacts.contacts_mass_offset,
			   environment.state.GPU_buffers.fluid.contacts.contacts_impulse_range_offset
			};
			environment.state.GPU_buffers.fluid.contacts.contacts_offset = *std::min_element(std::begin(offsets), std::end(offsets));
#if USE_DYNAMIC_SIZES == true
			environment.state.GPU_buffers.fluid.contacts.size = environment.state.GPU_buffers.fluid.contacts.contacts_offset + MAX_FLUID_CONTACT_COUNT(environment) * environment.state.GPU_buffers.fluid.contacts.contacts_stride;
#else
			GLuint const block_index
			{
				glGetProgramResourceIndex(environment.state.persist_fluid_contacts_shader, GL_SHADER_STORAGE_BLOCK, "Fluid_Contacts")
			};
			GLenum const buffer_size_label{ GL_BUFFER_DATA_SIZE };
			glGetProgramResourceiv
			(
				environment.state.persist_fluid_contacts_shader, GL_SHADER_STORAGE_BLOCK, block_index,
				1u, &buffer_size_label, 1u, nullptr, &environment.state.GPU_buffers.fluid.contacts.size
			);
#endif

			glNamedBufferStorage
			(
				environment.state.GPU_buffers.fluid.contacts.buffer, environment.state.GPU_buffers.fluid.contacts.size, nullptr,
				0u//GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT
			);

			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, game_logic__util_FLUID_CONTACT_BINDING, environment.state.GPU_buffers.fluid.contacts.buffer);

			/*environment.state.fluid_contact_mapping = static_cast<unsigned char*>
				(
					glMapNamedBufferRange
					(
						environment.state.GPU_buffers.fluid.contacts.buffer,
						0u, environment.state.GPU_buffers.fluid.contacts.size,
						GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT | GL_MAP_FLUSH_EXPLICIT_BIT
					)
					);*/
		}

		{ // Fluid contact count buffer
			GLenum offset_label{ GL_OFFSET };

			GLuint const count_index
			{
				glGetProgramResourceIndex(environment.state.persist_fluid_contacts_shader, GL_UNIFORM, "Fluid_Contact_Count.count")
			};
			glGetProgramResourceiv
			(
				environment.state.persist_fluid_contacts_shader, GL_UNIFORM, count_index,
				1u, &offset_label, 1u, nullptr, &environment.state.GPU_buffers.fluid.contact_count.count_offset
			);

			GLuint const block_index
			{
				glGetProgramResourceIndex(environment.state.persist_fluid_contacts_shader, GL_UNIFORM_BLOCK, "Fluid_Contact_Count")
			};
			GLenum const buffer_size_label{ GL_BUFFER_DATA_SIZE };
			glGetProgramResourceiv
			(
				environment.state.persist_fluid_contacts_shader, GL_UNIFORM_BLOCK, block_index,
				1u, &buffer_size_label, 1u, nullptr, &environment.state.GPU_buffers.fluid.contact_count.size
			);

			unsigned char* const initial_fluid_contact_count = new unsigned char[environment.state.GPU_buffers.fluid.contact_count.size];
			//std::memcpy(initial_fluid_contact_count + environment.state.fluid_contact_count_buffer_persistent_count_offset, &environment.state.current_fluid_contact_count, sizeof(GLuint));
			std::memcpy(initial_fluid_contact_count + environment.state.GPU_buffers.fluid.contact_count.count_offset, &environment.state.GPU_buffers.fluid.contact_count.current_contact_count, sizeof(GLuint));

			glNamedBufferStorage
			(
				environment.state.GPU_buffers.fluid.contact_count.buffer, environment.state.GPU_buffers.fluid.contact_count.size, initial_fluid_contact_count,
				0u
			);

			delete[] initial_fluid_contact_count;

			glBindBufferBase(GL_UNIFORM_BUFFER, game_logic__util_FLUID_CONTACT_COUNT_BINDING, environment.state.GPU_buffers.fluid.contact_count.buffer);
		}

		{ // Fluid velocity snapshot buffer
			glNamedBufferStorage
			(
				environment.state.GPU_buffers.fluid.velocities.snapshot_buffer, environment.state.GPU_buffers.fluid.velocities.size, nullptr,
				0u
			);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, game_logic__util_FLUID_VELOCITY_SNAPSHOT_BINDING, environment.state.GPU_buffers.fluid.velocities.snapshot_buffer);
		}

		{ // Fluid triangle contact buffer
			{
				GLuint const contacts_triangle_index
				{
					glGetProgramResourceIndex(environment.state.persist_fluid_triangle_contacts_shader, GL_BUFFER_VARIABLE, "Fluid_Triangle_Contacts.contacts[0].triangle")
				};
				GLenum const prop_labels[]{ GL_OFFSET, GL_TOP_LEVEL_ARRAY_STRIDE };
				GLint props[std::size(prop_labels)];
				glGetProgramResourceiv
				(
					environment.state.persist_fluid_triangle_contacts_shader, GL_BUFFER_VARIABLE, contacts_triangle_index,
					std::size(prop_labels), prop_labels, 2u, nullptr, props
				);
				// TODO: Consider putting offset and stride contigously in game state
				environment.state.GPU_buffers.fluid_triangle.contacts.contacts_triangle_offset = props[0u];
				environment.state.GPU_buffers.fluid_triangle.contacts.contacts_stride = props[1u];

				GLenum const offset_label{ GL_OFFSET };

				GLuint const mass_index
				{
					glGetProgramResourceIndex(environment.state.persist_fluid_triangle_contacts_shader, GL_BUFFER_VARIABLE, "Fluid_Triangle_Contacts.contacts[0].mass")
				};
				glGetProgramResourceiv
				(
					environment.state.persist_fluid_triangle_contacts_shader, GL_BUFFER_VARIABLE, mass_index,
					1u, &offset_label, 1u, nullptr, &environment.state.GPU_buffers.fluid_triangle.contacts.contacts_mass_offset
				);

				GLuint const impulse_index
				{
					glGetProgramResourceIndex(environment.state.persist_fluid_triangle_contacts_shader, GL_BUFFER_VARIABLE, "Fluid_Triangle_Contacts.contacts[0].impulse")
				};
				glGetProgramResourceiv
				(
					environment.state.persist_fluid_triangle_contacts_shader, GL_BUFFER_VARIABLE, impulse_index,
					1u, &offset_label, 1u, nullptr, &environment.state.GPU_buffers.fluid_triangle.contacts.contacts_impulse_offset
				);

				GLuint const particle_index
				{
					glGetProgramResourceIndex(environment.state.persist_fluid_triangle_contacts_shader, GL_BUFFER_VARIABLE, "Fluid_Triangle_Contacts.contacts[0].particle")
				};
				glGetProgramResourceiv
				(
					environment.state.persist_fluid_triangle_contacts_shader, GL_BUFFER_VARIABLE, particle_index,
					1u, &offset_label, 1u, nullptr, &environment.state.GPU_buffers.fluid_triangle.contacts.contacts_particle_offset
				);

				GLuint const body_index
				{
					glGetProgramResourceIndex(environment.state.persist_fluid_triangle_contacts_shader, GL_BUFFER_VARIABLE, "Fluid_Triangle_Contacts.contacts[0].body")
				};
				glGetProgramResourceiv
				(
					environment.state.persist_fluid_triangle_contacts_shader, GL_BUFFER_VARIABLE, body_index,
					1u, &offset_label, 1u, nullptr, &environment.state.GPU_buffers.fluid_triangle.contacts.contacts_body_offset
				);

				GLuint const target_velocity_index
				{
					glGetProgramResourceIndex(environment.state.persist_fluid_triangle_contacts_shader, GL_BUFFER_VARIABLE, "Fluid_Triangle_Contacts.contacts[0].target_velocity")
				};
				glGetProgramResourceiv
				(
					environment.state.persist_fluid_triangle_contacts_shader, GL_BUFFER_VARIABLE, target_velocity_index,
					1u, &offset_label, 1u, nullptr, &environment.state.GPU_buffers.fluid_triangle.contacts.contacts_target_velocity_offset
				);

				GLuint const offset_index
				{
					glGetProgramResourceIndex(environment.state.persist_fluid_triangle_contacts_shader, GL_BUFFER_VARIABLE, "Fluid_Triangle_Contacts.contacts[0].offset")
				};
				glGetProgramResourceiv
				(
					environment.state.persist_fluid_triangle_contacts_shader, GL_BUFFER_VARIABLE, offset_index,
					1u, &offset_label, 1u, nullptr, &environment.state.GPU_buffers.fluid_triangle.contacts.contacts_offset_offset
				);

				GLuint const direction_index
				{
					glGetProgramResourceIndex(environment.state.persist_fluid_triangle_contacts_shader, GL_BUFFER_VARIABLE, "Fluid_Triangle_Contacts.contacts[0].direction")
				};
				glGetProgramResourceiv
				(
					environment.state.persist_fluid_triangle_contacts_shader, GL_BUFFER_VARIABLE, direction_index,
					1u, &offset_label, 1u, nullptr, &environment.state.GPU_buffers.fluid_triangle.contacts.contacts_direction_offset
				);

				GLuint const impulse_range_index
				{
					glGetProgramResourceIndex(environment.state.persist_fluid_triangle_contacts_shader, GL_BUFFER_VARIABLE, "Fluid_Triangle_Contacts.contacts[0].impulse_range")
				};
				glGetProgramResourceiv
				(
					environment.state.persist_fluid_triangle_contacts_shader, GL_BUFFER_VARIABLE, impulse_range_index,
					1u, &offset_label, 1u, nullptr, &environment.state.GPU_buffers.fluid_triangle.contacts.contacts_impulse_range_offset
				);
			}

			GLint const offsets[]
			{
			   environment.state.GPU_buffers.fluid_triangle.contacts.contacts_triangle_offset,
			   environment.state.GPU_buffers.fluid_triangle.contacts.contacts_mass_offset,
			   environment.state.GPU_buffers.fluid_triangle.contacts.contacts_impulse_offset,
			   environment.state.GPU_buffers.fluid_triangle.contacts.contacts_particle_offset,
			   environment.state.GPU_buffers.fluid_triangle.contacts.contacts_body_offset,
			   environment.state.GPU_buffers.fluid_triangle.contacts.contacts_target_velocity_offset,
			   environment.state.GPU_buffers.fluid_triangle.contacts.contacts_offset_offset,
			   environment.state.GPU_buffers.fluid_triangle.contacts.contacts_direction_offset,
			   environment.state.GPU_buffers.fluid_triangle.contacts.contacts_impulse_range_offset
			};
			environment.state.GPU_buffers.fluid_triangle.contacts.contacts_offset = *std::min_element(std::begin(offsets), std::end(offsets));
#if USE_DYNAMIC_SIZES == true
			environment.state.GPU_buffers.fluid_triangle.contacts.size = environment.state.GPU_buffers.fluid_triangle.contacts.contacts_offset + MAX_FLUID_TRIANGLE_CONTACT_COUNT(environment) * environment.state.GPU_buffers.fluid_triangle.contacts.contacts_stride;
#else
			GLuint const block_index
			{
				glGetProgramResourceIndex(environment.state.persist_fluid_triangle_contacts_shader, GL_SHADER_STORAGE_BLOCK, "Fluid_Triangle_Contacts")
			};
			GLenum const buffer_size_label{ GL_BUFFER_DATA_SIZE };
			glGetProgramResourceiv
			(
				environment.state.persist_fluid_triangle_contacts_shader, GL_SHADER_STORAGE_BLOCK, block_index,
				1u, &buffer_size_label, 1u, nullptr, &environment.state.fluid_triangle_contact_buffer_size
			);
#endif

			glNamedBufferStorage
			(
				environment.state.GPU_buffers.fluid_triangle.contacts.buffer, environment.state.GPU_buffers.fluid_triangle.contacts.size, nullptr,
				0u
			);

			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, game_logic__util_FLUID_TRIANGLE_CONTACT_BINDING, environment.state.GPU_buffers.fluid_triangle.contacts.buffer);
		}

		{ // Fluid triangle contact count buffer
			GLenum offset_label{ GL_OFFSET };

			GLuint const count_index
			{
				glGetProgramResourceIndex(environment.state.persist_fluid_triangle_contacts_shader, GL_UNIFORM, "Fluid_Triangle_Contact_Count.count")
			};
			glGetProgramResourceiv
			(
				environment.state.persist_fluid_triangle_contacts_shader, GL_UNIFORM, count_index,
				1u, &offset_label, 1u, nullptr, &environment.state.GPU_buffers.fluid_triangle.contact_count.count_offset
			);

			GLuint const block_index
			{
				glGetProgramResourceIndex(environment.state.persist_fluid_triangle_contacts_shader, GL_UNIFORM_BLOCK, "Fluid_Triangle_Contact_Count")
			};
			GLenum const buffer_size_label{ GL_BUFFER_DATA_SIZE };
			glGetProgramResourceiv
			(
				environment.state.persist_fluid_triangle_contacts_shader, GL_UNIFORM_BLOCK, block_index,
				1u, &buffer_size_label, 1u, nullptr, &environment.state.GPU_buffers.fluid_triangle.contact_count.size
			);

			unsigned char* const initial_fluid_triangle_contact_count = new unsigned char[environment.state.GPU_buffers.fluid_triangle.contact_count.size];
			//std::memcpy(initial_fluid_contact_count + environment.state.fluid_contact_count_buffer_persistent_count_offset, &environment.state.current_fluid_contact_count, sizeof(GLuint));
			std::memcpy(initial_fluid_triangle_contact_count + environment.state.GPU_buffers.fluid_triangle.contact_count.count_offset, &environment.state.GPU_buffers.fluid_triangle.contact_count.current_contact_count, sizeof(GLuint));

			glNamedBufferStorage
			(
				environment.state.GPU_buffers.fluid_triangle.contact_count.buffer, environment.state.GPU_buffers.fluid_triangle.contact_count.size, initial_fluid_triangle_contact_count,
				0u
			);

			delete[] initial_fluid_triangle_contact_count;

			glBindBufferBase(GL_UNIFORM_BUFFER, game_logic__util_FLUID_TRIANGLE_CONTACT_COUNT_BINDING, environment.state.GPU_buffers.fluid_triangle.contact_count.buffer);
		}

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
			environment.state.GPU_buffers.rigid_bodies.positions.p_offset = props[0];
			environment.state.GPU_buffers.rigid_bodies.positions.p_stride = props[1];

#if USE_DYNAMIC_SIZES == true
			environment.state.GPU_buffers.rigid_bodies.positions.size = environment.state.GPU_buffers.rigid_bodies.positions.p_offset + game_logic_MAX_RIGID_BODY_COUNT(environment) * environment.state.GPU_buffers.rigid_bodies.positions.p_stride;
#else
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
#endif

			// TODO: Don't initialize a few positions by copying over the ENTIRE buffer 
			// content from CPU to GPU like this. Instead, use persistent mapping 
			// for both initialization and updating.
			unsigned char* const initial_positions = new unsigned char[environment.state.GPU_buffers.rigid_bodies.positions.size];
			
			for (GLuint i = 0; i < environment.state.current_rigid_body_count; ++i)
			{
				GLuint const width{ 100u };
				util::rigid_body::Position position
				{
					{
						game_logic__util__spatial_FROM_METERS(environment, (i % width) * 0.2f),
						game_logic__util__spatial_FROM_METERS(environment, (i / width) * 0.2f)
					}, 
					game_logic__util__spatial_FROM_RADIANS(environment, i * 0.1f), 
					0
				};
				position.position.x *= 10.0f;
				position.position.y *= 10.0f;
				if (i < 1)
				{
					position.position.x += game_logic__util__spatial_FROM_METERS(environment, -10.0f);
				}
				if (i < 2)
				{
					//position.position.x += game_logic__util__spatial_FROM_METERS(environment, -10.0f);
					position.position.x = game_logic__util__spatial_FROM_METERS(environment, i * (-0.1));
					position.position.y = 0;
				}
				//position.angle = 0;
				std::memcpy
				(
					initial_positions + environment.state.GPU_buffers.rigid_bodies.positions.p_offset + i * environment.state.GPU_buffers.rigid_bodies.positions.p_stride,
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

			// VERY IMPORTANT TODO: REMOVE BIT-FLAGS!!! I think this is the reason the rigid body velocity integration
			// is slower than the fluid one, and also a main reason why the triangle-triangle/triangle-fluid contact
			// detection shaders are slow.
			glNamedBufferStorage
			(
				environment.state.GPU_buffers.rigid_bodies.positions.buffer, environment.state.GPU_buffers.rigid_bodies.positions.size, initial_positions,
				GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT //0u
			);

			delete[] initial_positions;

			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, game_logic__util_RIGID_BODY_POSITION_BINDING, environment.state.GPU_buffers.rigid_bodies.positions.buffer);

			environment.state.GPU_buffers.rigid_bodies.positions.mapping = static_cast<unsigned char*>
			(
				glMapNamedBufferRange
				(
					environment.state.GPU_buffers.rigid_bodies.positions.buffer,
					0u, environment.state.GPU_buffers.rigid_bodies.positions.size,
					GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_FLUSH_EXPLICIT_BIT
				)
			);
		}

		{ // Position snapshot buffer
			glNamedBufferStorage
			(
				environment.state.GPU_buffers.rigid_bodies.positions.snapshot_buffer, environment.state.GPU_buffers.rigid_bodies.positions.size, nullptr,
				0u
			);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, game_logic__util_POSITION_SNAPSHOT_BINDING, environment.state.GPU_buffers.rigid_bodies.positions.snapshot_buffer);
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
			environment.state.GPU_buffers.rigid_bodies.velocities.v_offset = props[0];
			environment.state.GPU_buffers.rigid_bodies.velocities.v_stride = props[1];

#if USE_DYNAMIC_SIZES == true
			environment.state.GPU_buffers.rigid_bodies.velocities.size = environment.state.GPU_buffers.rigid_bodies.velocities.v_offset + game_logic_MAX_RIGID_BODY_COUNT(environment) * environment.state.GPU_buffers.rigid_bodies.velocities.v_stride;
#else
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
#endif
			// TODO: Don't initialize a few positions by copying over the ENTIRE buffer 
			// content from CPU to GPU like this. Instead, use persistent mapping 
			// for both initialization and updating.
			unsigned char* const initial_velocities = new unsigned char[environment.state.GPU_buffers.rigid_bodies.velocities.size];

			for (GLuint i = 0; i < environment.state.current_rigid_body_count; ++i)
			{
				util::rigid_body::Velocity velocity
				{
					{
						2 * game_METERS_PER_SECOND_TO_LENGTH_PER_TICK(environment, -sin(i * 0.1f)),
						2 * game_METERS_PER_SECOND_TO_LENGTH_PER_TICK(environment, cos(i * 0.1f))
					},
					game_RADIANS_PER_SECOND_TO_ANGLE_PER_TICK(environment, 0.5f), 
					0
				};
				if (i == 1u)
				{
					velocity.velocity.x = game_METERS_PER_SECOND_TO_LENGTH_PER_TICK(environment, 2.1f);
					velocity.velocity.y = game_METERS_PER_SECOND_TO_LENGTH_PER_TICK(environment, 0.2f);
				}
				if (i == 2u)
				{
					velocity.velocity.x = game_METERS_PER_SECOND_TO_LENGTH_PER_TICK(environment, 0.0f);
					velocity.velocity.y = game_METERS_PER_SECOND_TO_LENGTH_PER_TICK(environment, 0.0f);
				}

				if (i < 2u)
				{
					velocity.velocity.x = 0;
					velocity.velocity.y = 0;
				}

				velocity.angle_velocity = 0;
				/*if (2 <= i && i < environment.state.current_rigid_body_count - 2)
				{
					velocity.velocity.x = 0;
					velocity.velocity.y = 0;
					velocity.angle_velocity = 0;
				}*/
				std::memcpy
				(
					initial_velocities + environment.state.GPU_buffers.rigid_bodies.velocities.v_offset + i * environment.state.GPU_buffers.rigid_bodies.velocities.v_stride,
					&velocity, sizeof(velocity)
				);
			}

			glNamedBufferStorage
			(
				environment.state.GPU_buffers.rigid_bodies.velocities.buffer, environment.state.GPU_buffers.rigid_bodies.velocities.size, initial_velocities,
				0u
			);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, game_logic__util_RIGID_BODY_VELOCITY_BINDING, environment.state.GPU_buffers.rigid_bodies.velocities.buffer);

			delete[] initial_velocities;
		}

		{ // Velocity snapshot buffer
			glNamedBufferStorage
			(
				environment.state.GPU_buffers.rigid_bodies.velocities.snapshot_buffer, environment.state.GPU_buffers.rigid_bodies.velocities.size, nullptr,
				0u
			);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, game_logic__util_VELOCITY_SNAPSHOT_BINDING, environment.state.GPU_buffers.rigid_bodies.velocities.snapshot_buffer);
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
			environment.state.GPU_buffers.rigid_bodies.triangles.triangles_offset = props[0];
			environment.state.GPU_buffers.rigid_bodies.triangles.triangles_stride = props[1];

#if USE_DYNAMIC_SIZES == true
			environment.state.GPU_buffers.rigid_bodies.triangles.size = environment.state.GPU_buffers.rigid_bodies.triangles.triangles_offset + game_logic_MAX_TRIANGLE_COUNT(environment) * environment.state.GPU_buffers.rigid_bodies.triangles.triangles_stride;
#else
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
#endif

			// TODO: Don't initialize a few triangles by copying over the ENTIRE buffer 
			// content from CPU to GPU like this. Instead, use persistent mapping 
			// for both initialization and updating.
			unsigned char* const initial_triangles = new unsigned char[environment.state.GPU_buffers.rigid_bodies.triangles.size];
			environment.state.GPU_buffers.rigid_bodies.triangles.values = new game_state::rigid_body::Triangle[game_logic_MAX_TRIANGLE_COUNT(environment)];

			for (GLuint i = 0; i < environment.state.GPU_buffers.rigid_bodies.triangles.current_count; ++i)
			{
				/*util::rigid_body::Triangle triangle
				{
					{(2u * i) % 4u, (2u * i + 1u) % 4u, (2u * i + 2u) % 4u}, i / 2u
				};*/
				game_state::rigid_body::Triangle triangle
				{
					{(2u * i) % 4u, (2u * i + 1u) % 4u, (2u * i + 2u) % 4u}, i
				};
				std::memcpy
				(
					initial_triangles + environment.state.GPU_buffers.rigid_bodies.triangles.triangles_offset + i * environment.state.GPU_buffers.rigid_bodies.triangles.triangles_stride,
					&triangle, sizeof(triangle)
				);

				environment.state.GPU_buffers.rigid_bodies.triangles.values[i] = triangle;
			}

			glNamedBufferStorage
			(
				environment.state.GPU_buffers.rigid_bodies.triangles.buffer, environment.state.GPU_buffers.rigid_bodies.triangles.size, initial_triangles,
				0u
			);

			delete[] initial_triangles;

			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, game_logic__util_TRIANGLE_BINDING, environment.state.GPU_buffers.rigid_bodies.triangles.buffer);
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
			environment.state.GPU_buffers.rigid_bodies.triangles.vertices.vertices_offset = props[0];
			environment.state.GPU_buffers.rigid_bodies.triangles.vertices.vertices_stride = props[1];

#if USE_DYNAMIC_SIZES == true
			environment.state.GPU_buffers.rigid_bodies.triangles.vertices.size = environment.state.GPU_buffers.rigid_bodies.triangles.vertices.vertices_offset + game_logic_MAX_VERTEX_COUNT(environment) * environment.state.GPU_buffers.rigid_bodies.triangles.vertices.vertices_stride;
#else
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
#endif
			// TODO: Don't initialize a few vertices by copying over the ENTIRE buffer 
			// content from CPU to GPU like this. Instead, use persistent mapping 
			// for both initialization and updating.
			unsigned char* const initial_vertices = new unsigned char[environment.state.GPU_buffers.rigid_bodies.triangles.vertices.size];
			
			GLfloat vertex[2];
			GLfloat const r{ 0.5f };
			vertex[0] = game_logic__util__spatial_FLOAT_FROM_METERS(environment, r);
			vertex[1] = game_logic__util__spatial_FLOAT_FROM_METERS(environment, r);
			std::memcpy
			(
				initial_vertices + environment.state.GPU_buffers.rigid_bodies.triangles.vertices.vertices_offset + 0 * environment.state.GPU_buffers.rigid_bodies.triangles.vertices.vertices_stride,
				&vertex, sizeof(vertex)
			);
			environment.state.GPU_buffers.rigid_bodies.triangles.vertices.values[0u][0u] = vertex[0u];
			environment.state.GPU_buffers.rigid_bodies.triangles.vertices.values[0u][1u] = vertex[1u];

			vertex[0] = game_logic__util__spatial_FLOAT_FROM_METERS(environment, -r);
			vertex[1] = game_logic__util__spatial_FLOAT_FROM_METERS(environment, r);
			std::memcpy
			(
				initial_vertices + environment.state.GPU_buffers.rigid_bodies.triangles.vertices.vertices_offset + 1 * environment.state.GPU_buffers.rigid_bodies.triangles.vertices.vertices_stride,
				&vertex, sizeof(vertex)
			);
			environment.state.GPU_buffers.rigid_bodies.triangles.vertices.values[1u][0u] = vertex[0u];
			environment.state.GPU_buffers.rigid_bodies.triangles.vertices.values[1u][1u] = vertex[1u];

			vertex[0] = game_logic__util__spatial_FLOAT_FROM_METERS(environment, -r);
			vertex[1] = game_logic__util__spatial_FLOAT_FROM_METERS(environment, -r);
			std::memcpy
			(
				initial_vertices + environment.state.GPU_buffers.rigid_bodies.triangles.vertices.vertices_offset + 2 * environment.state.GPU_buffers.rigid_bodies.triangles.vertices.vertices_stride,
				&vertex, sizeof(vertex)
			);
			environment.state.GPU_buffers.rigid_bodies.triangles.vertices.values[2u][0u] = vertex[0u];
			environment.state.GPU_buffers.rigid_bodies.triangles.vertices.values[2u][1u] = vertex[1u];

			vertex[0] = game_logic__util__spatial_FLOAT_FROM_METERS(environment, r);
			vertex[1] = game_logic__util__spatial_FLOAT_FROM_METERS(environment, -r);
			std::memcpy
			(
				initial_vertices + environment.state.GPU_buffers.rigid_bodies.triangles.vertices.vertices_offset + 3 * environment.state.GPU_buffers.rigid_bodies.triangles.vertices.vertices_stride,
				&vertex, sizeof(vertex)
			);
			environment.state.GPU_buffers.rigid_bodies.triangles.vertices.values[3u][0u] = vertex[0u];
			environment.state.GPU_buffers.rigid_bodies.triangles.vertices.values[3u][1u] = vertex[1u];

			

			/*for (GLuint i = 0; i < environment.state.GPU_buffers.rigid_bodies.triangles.current_count; ++i)
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
				environment.state.GPU_buffers.rigid_bodies.triangles.vertices.buffer, environment.state.GPU_buffers.rigid_bodies.triangles.vertices.size, initial_vertices,
				0u
			);

			delete[] initial_vertices;

			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, game_logic__util_VERTEX_BINDING, environment.state.GPU_buffers.rigid_bodies.triangles.vertices.buffer);
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
			environment.state.GPU_buffers.rigid_bodies.triangles.bounding_boxes.boxes_offset = props[0];
			environment.state.GPU_buffers.rigid_bodies.triangles.bounding_boxes.boxes_stride = props[1];

#if USE_DYNAMIC_SIZES == true
			environment.state.GPU_buffers.rigid_bodies.triangles.bounding_boxes.size = environment.state.GPU_buffers.rigid_bodies.triangles.bounding_boxes.boxes_offset + game_logic_MAX_TRIANGLE_COUNT(environment) * environment.state.GPU_buffers.rigid_bodies.triangles.bounding_boxes.boxes_stride;
#else
			GLuint const block_index
			{
				glGetProgramResourceIndex(environment.state.triangle_bounding_box_update_shader, GL_SHADER_STORAGE_BLOCK, "Bounding_Boxes")
			};
			GLenum const buffer_size_label{ GL_BUFFER_DATA_SIZE };
			glGetProgramResourceiv
			(
				environment.state.triangle_bounding_box_update_shader, GL_SHADER_STORAGE_BLOCK, block_index,
				1, &buffer_size_label, 1, nullptr, &environment.state.GPU_buffers.rigid_bodies.triangles.bounding_boxes.size
			);
#endif

			// TODO: Don't initialize a few boxes by copying over the ENTIRE buffer 
			// content from CPU to GPU like this. Instead, use persistent mapping 
			// for both initialization and updating.
			unsigned char* const initial_boxes = new unsigned char[environment.state.GPU_buffers.rigid_bodies.triangles.bounding_boxes.size];

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
			for (GLuint i = 0; i < environment.state.GPU_buffers.rigid_bodies.triangles.current_count; ++i)
			{
				std::memcpy
				(
					initial_boxes + environment.state.GPU_buffers.rigid_bodies.triangles.bounding_boxes.boxes_offset + i * environment.state.GPU_buffers.rigid_bodies.triangles.bounding_boxes.boxes_stride,
					&box, sizeof(box)
				);
			}

			glNamedBufferStorage
			(
				environment.state.GPU_buffers.rigid_bodies.triangles.bounding_boxes.buffer, environment.state.GPU_buffers.rigid_bodies.triangles.bounding_boxes.size, initial_boxes, 
				0u
			);

			delete[] initial_boxes;

			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, game_logic__util_TRIANGLE_BOUNDING_BOX_BINDING, environment.state.GPU_buffers.rigid_bodies.triangles.bounding_boxes.buffer);
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
					 1, &offset_label, 1, nullptr, &environment.state.GPU_buffers.rigid_bodies.triangles.changed_bounding_boxes.size_offset
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
				 environment.state.GPU_buffers.rigid_bodies.triangles.changed_bounding_boxes.boxes_index_offset = props[0];
				 environment.state.GPU_buffers.rigid_bodies.triangles.changed_bounding_boxes.boxes_stride = props[1];

				 GLenum const offset_label{ GL_OFFSET };

				 GLuint const boxes_min_x_index
				 {
					 glGetProgramResourceIndex(environment.state.triangle_bounding_box_update_shader, GL_BUFFER_VARIABLE, "Changed_Bounding_Boxes.boxes[0].min_x")
				 };
				 glGetProgramResourceiv
				 (
					 environment.state.triangle_bounding_box_update_shader, GL_BUFFER_VARIABLE, boxes_min_x_index,
					 1, &offset_label, 1, nullptr, &environment.state.GPU_buffers.rigid_bodies.triangles.changed_bounding_boxes.boxes_min_x_offset
				 );

				 GLuint const boxes_min_y_index
				 {
					 glGetProgramResourceIndex(environment.state.triangle_bounding_box_update_shader, GL_BUFFER_VARIABLE, "Changed_Bounding_Boxes.boxes[0].min_y")
				 };
				 glGetProgramResourceiv
				 (
					 environment.state.triangle_bounding_box_update_shader, GL_BUFFER_VARIABLE, boxes_min_y_index,
					 1, &offset_label, 1, nullptr, &environment.state.GPU_buffers.rigid_bodies.triangles.changed_bounding_boxes.boxes_min_y_offset
				 );

				 GLuint const boxes_max_x_index
				 {
					 glGetProgramResourceIndex(environment.state.triangle_bounding_box_update_shader, GL_BUFFER_VARIABLE, "Changed_Bounding_Boxes.boxes[0].max_x")
				 };
				 glGetProgramResourceiv
				 (
					 environment.state.triangle_bounding_box_update_shader, GL_BUFFER_VARIABLE, boxes_max_x_index,
					 1, &offset_label, 1, nullptr, &environment.state.GPU_buffers.rigid_bodies.triangles.changed_bounding_boxes.boxes_max_x_offset
				 );

				 GLuint const boxes_max_y_index
				 {
					 glGetProgramResourceIndex(environment.state.triangle_bounding_box_update_shader, GL_BUFFER_VARIABLE, "Changed_Bounding_Boxes.boxes[0].max_y")
				 };
				 glGetProgramResourceiv
				 (
					 environment.state.triangle_bounding_box_update_shader, GL_BUFFER_VARIABLE, boxes_max_y_index,
					 1, &offset_label, 1, nullptr, &environment.state.GPU_buffers.rigid_bodies.triangles.changed_bounding_boxes.boxes_max_y_offset
				 );
			}

#if USE_DYNAMIC_SIZES == true
			 GLint const offsets[]
			 {
				environment.state.GPU_buffers.rigid_bodies.triangles.changed_bounding_boxes.boxes_index_offset,
				environment.state.GPU_buffers.rigid_bodies.triangles.changed_bounding_boxes.boxes_min_x_offset,
				environment.state.GPU_buffers.rigid_bodies.triangles.changed_bounding_boxes.boxes_min_y_offset,
				environment.state.GPU_buffers.rigid_bodies.triangles.changed_bounding_boxes.boxes_max_x_offset,
				environment.state.GPU_buffers.rigid_bodies.triangles.changed_bounding_boxes.boxes_max_y_offset
			 };
			 GLint const offset{ *std::min_element(std::begin(offsets), std::end(offsets)) };
			 environment.state.GPU_buffers.rigid_bodies.triangles.changed_bounding_boxes.size = offset + game_logic_MAX_TRIANGLE_COUNT(environment) * environment.state.GPU_buffers.rigid_bodies.triangles.changed_bounding_boxes.boxes_stride;
#else
			 GLuint const block_index
			 {
				 glGetProgramResourceIndex(environment.state.triangle_bounding_box_update_shader, GL_SHADER_STORAGE_BLOCK, "Changed_Bounding_Boxes")
			 };
			 GLenum const buffer_size_label{ GL_BUFFER_DATA_SIZE };
			 glGetProgramResourceiv
			 (
				 environment.state.triangle_bounding_box_update_shader, GL_SHADER_STORAGE_BLOCK, block_index,
				 1, &buffer_size_label, 1, nullptr, &environment.state.GPU_buffers.rigid_bodies.triangles.changed_bounding_boxes.buffer_size
			 );
#endif

			 glNamedBufferStorage
			 (
				 environment.state.GPU_buffers.rigid_bodies.triangles.changed_bounding_boxes.buffer, environment.state.GPU_buffers.rigid_bodies.triangles.changed_bounding_boxes.size, nullptr,
				 GL_MAP_READ_BIT | GL_MAP_PERSISTENT_BIT
			 );
			 glClearNamedBufferSubData
			 (
				 environment.state.GPU_buffers.rigid_bodies.triangles.changed_bounding_boxes.buffer, 
				 GL_R32UI, 
				 environment.state.GPU_buffers.rigid_bodies.triangles.changed_bounding_boxes.size_offset, sizeof(GLuint), 
				 GL_RED_INTEGER, GL_UNSIGNED_INT, 
				 nullptr
			 );

			 glBindBufferBase(GL_SHADER_STORAGE_BUFFER, game_logic__util_TRIANGLE_CHANGED_BOUNDING_BOX_BINDING, environment.state.GPU_buffers.rigid_bodies.triangles.changed_bounding_boxes.buffer);

			 environment.state.changed_bounding_boxes_mapping = static_cast<unsigned char*>
			 (
				glMapNamedBufferRange
				(
					environment.state.GPU_buffers.rigid_bodies.triangles.changed_bounding_boxes.buffer, 
					0u, environment.state.GPU_buffers.rigid_bodies.triangles.changed_bounding_boxes.size, 
					GL_MAP_READ_BIT | GL_MAP_PERSISTENT_BIT
				)
			 );
		 }

		 { // Contact buffer
			 GLuint const contacts_index
			 {
				 glGetProgramResourceIndex(environment.state.leaf_triangle_contact_draw_shader, GL_BUFFER_VARIABLE, "Contacts.contacts")
			 };

			 GLenum const prop_labels[]{ GL_OFFSET, GL_ARRAY_STRIDE };
			 GLint props[std::size(prop_labels)];
			 glGetProgramResourceiv
			 (
				 environment.state.leaf_triangle_contact_draw_shader, GL_BUFFER_VARIABLE, contacts_index,
				 std::size(prop_labels), prop_labels, 2, nullptr, props
			 );
			 // TODO: Consider putting offset and stride contigously in game state
			 environment.state.contact_buffer_contacts_offset = props[0];
			 environment.state.contact_buffer_contacts_stride = props[1];

#if USE_DYNAMIC_SIZES == true
			 environment.state.contact_buffer_size = environment.state.contact_buffer_contacts_offset + game_logic_MAX_TRIANGLE_CONTACT_COUNT(environment) * environment.state.contact_buffer_contacts_stride;
#else
			 GLuint const block_index
			 {
				 glGetProgramResourceIndex(environment.state.leaf_contact_draw_shader, GL_SHADER_STORAGE_BLOCK, "Contacts")
			 };
			 GLenum const buffer_size_label{ GL_BUFFER_DATA_SIZE };
			 glGetProgramResourceiv
			 (
				 environment.state.leaf_contact_draw_shader, GL_SHADER_STORAGE_BLOCK, block_index,
				 1, &buffer_size_label, 1, nullptr, &environment.state.contact_buffer_size
			 );
#endif

			 // TODO: Don't initialize a few contacts by copying over the ENTIRE buffer 
			 // content from CPU to GPU like this. Instead, use persistent mapping 
			 // for both initialization and updating.
			 unsigned char* const initial_contacts = new unsigned char[environment.state.contact_buffer_size];

			 for (GLuint i{ 0u }; i < environment.state.current_triangle_contact_count; ++i)
			 {
				 GLuint contact[2]{ i, i + 1u };
				 std::memcpy
				 (
					 initial_contacts + environment.state.contact_buffer_contacts_offset + i * environment.state.contact_buffer_contacts_stride,
					 contact, sizeof(contact)
				 );
			 }

			 // IMPORTANT TODO: Remove the bit flags, since we noticed a significant performance penalty 
			 // when using flags for fluid buffers. This might be one of the primary reasons the 
			 // triangle-triangle contact detection is slow.
			 glNamedBufferStorage
			 (
				 environment.state.contact_buffer, environment.state.contact_buffer_size, initial_contacts,
				 0u
				 //GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT
			 );

			 delete[] initial_contacts;

			 glBindBufferBase(GL_SHADER_STORAGE_BUFFER, game_logic__util_CONTACT_BINDING, environment.state.contact_buffer);

			 /*environment.state.contact_mapping = static_cast<unsigned char*>
			(
				glMapNamedBufferRange
				(
					environment.state.contact_buffer, 
					0u, environment.state.contact_buffer_size, 
					0u//GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_FLUSH_EXPLICIT_BIT
				)
			);*/
		}

		{ // Contact surface buffer
			{
				GLuint const bodies_index
				{
					glGetProgramResourceIndex(environment.state.old_triangle_contact_update_shader, GL_BUFFER_VARIABLE, "Contact_Surfaces.contact_surfaces[0].bodies[0]")
				};
				GLenum const prop_labels[]{ GL_OFFSET, GL_TOP_LEVEL_ARRAY_STRIDE, GL_ARRAY_STRIDE };
				GLint props[std::size(prop_labels)];
				glGetProgramResourceiv
				(
					environment.state.old_triangle_contact_update_shader, GL_BUFFER_VARIABLE, bodies_index, 
					std::size(prop_labels), prop_labels, 3u, nullptr, props
				);
				// TODO: Consider putting offset and strides contigously in game state
				environment.state.GPU_buffers.rigid_bodies.triangles.contact_surfaces.contact_surfaces_bodies_offset = props[0];
				environment.state.contact_surface_buffer_contact_surfaces_stride = props[1];
				environment.state.GPU_buffers.rigid_bodies.triangles.contact_surfaces.contact_surfaces_bodies_stride = props[2];

				GLenum const array_prop_labels[]{ GL_OFFSET, GL_ARRAY_STRIDE };
				GLint array_props[std::size(array_prop_labels)];

				GLuint const contact_surfaces_contact_point_position_0_offsets_index
				{
					glGetProgramResourceIndex(environment.state.old_triangle_contact_update_shader, GL_BUFFER_VARIABLE, "Contact_Surfaces.contact_surfaces[0].contact_point_positions[0].offsets[0]")
				};
				glGetProgramResourceiv
				(
					environment.state.old_triangle_contact_update_shader, GL_BUFFER_VARIABLE, contact_surfaces_contact_point_position_0_offsets_index,
					std::size(array_prop_labels), array_prop_labels, 2u, nullptr, array_props
				);
				environment.state.GPU_buffers.rigid_bodies.triangles.contact_surfaces.contact_surfaces_contact_point_position_0_offsets_offset = array_props[0];
				environment.state.GPU_buffers.rigid_bodies.triangles.contact_surfaces.contact_surfaces_contact_point_position_0_offsets_stride = array_props[1];

				GLuint const contact_surfaces_contact_point_position_1_offsets_index
				{
					glGetProgramResourceIndex(environment.state.old_triangle_contact_update_shader, GL_BUFFER_VARIABLE, "Contact_Surfaces.contact_surfaces[0].contact_point_positions[1].offsets[0]")
				};
				glGetProgramResourceiv
				(
					environment.state.old_triangle_contact_update_shader, GL_BUFFER_VARIABLE, contact_surfaces_contact_point_position_1_offsets_index,
					std::size(array_prop_labels), array_prop_labels, 2u, nullptr, array_props
				);
				environment.state.GPU_buffers.rigid_bodies.triangles.contact_surfaces.contact_surfaces_contact_point_position_1_offsets_offset = array_props[0];
				environment.state.GPU_buffers.rigid_bodies.triangles.contact_surfaces.contact_surfaces_contact_point_position_1_offsets_stride = array_props[1];

				GLenum const offset_label{ GL_OFFSET };

				GLuint const contact_surfaces_tangent_index
				{
					glGetProgramResourceIndex(environment.state.old_triangle_contact_update_shader, GL_BUFFER_VARIABLE, "Contact_Surfaces.contact_surfaces[0].tangent")
				};
				glGetProgramResourceiv
				(
					environment.state.old_triangle_contact_update_shader, GL_BUFFER_VARIABLE, contact_surfaces_tangent_index,
					1u, &offset_label, 1u, nullptr, &environment.state.GPU_buffers.rigid_bodies.triangles.contact_surfaces.contact_surfaces_tangent_offset
				);

				GLuint const contact_surfaces_contact_point_tangent_0_mass_index
				{
					glGetProgramResourceIndex(environment.state.old_triangle_contact_update_shader, GL_BUFFER_VARIABLE, "Contact_Surfaces.contact_surfaces[0].contact_point_tangents[0].mass")
				};
				glGetProgramResourceiv
				(
					environment.state.old_triangle_contact_update_shader, GL_BUFFER_VARIABLE, contact_surfaces_contact_point_tangent_0_mass_index,
					1u, &offset_label, 1u, nullptr, &environment.state.GPU_buffers.rigid_bodies.triangles.contact_surfaces.contact_surfaces_contact_point_tangent_0_mass_offset
				);

				GLuint const contact_surfaces_contact_point_tangent_0_impulse_index
				{
					glGetProgramResourceIndex(environment.state.old_triangle_contact_update_shader, GL_BUFFER_VARIABLE, "Contact_Surfaces.contact_surfaces[0].contact_point_tangents[0].impulse")
				};
				glGetProgramResourceiv
				(
					environment.state.old_triangle_contact_update_shader, GL_BUFFER_VARIABLE, contact_surfaces_contact_point_tangent_0_impulse_index,
					1u, &offset_label, 1u, nullptr, &environment.state.GPU_buffers.rigid_bodies.triangles.contact_surfaces.contact_surfaces_contact_point_tangent_0_impulse_offset
				);

				GLuint const contact_surfaces_contact_point_tangent_1_mass_index
				{
					glGetProgramResourceIndex(environment.state.old_triangle_contact_update_shader, GL_BUFFER_VARIABLE, "Contact_Surfaces.contact_surfaces[0].contact_point_tangents[1].mass")
				};
				glGetProgramResourceiv
				(
					environment.state.old_triangle_contact_update_shader, GL_BUFFER_VARIABLE, contact_surfaces_contact_point_tangent_1_mass_index,
					1u, &offset_label, 1u, nullptr, &environment.state.GPU_buffers.rigid_bodies.triangles.contact_surfaces.contact_surfaces_contact_point_tangent_1_mass_offset
				);

				GLuint const contact_surfaces_contact_point_tangent_1_impulse_index
				{
					glGetProgramResourceIndex(environment.state.old_triangle_contact_update_shader, GL_BUFFER_VARIABLE, "Contact_Surfaces.contact_surfaces[0].contact_point_tangents[1].impulse")
				};
				glGetProgramResourceiv
				(
					environment.state.old_triangle_contact_update_shader, GL_BUFFER_VARIABLE, contact_surfaces_contact_point_tangent_1_impulse_index,
					1u, &offset_label, 1u, nullptr, &environment.state.GPU_buffers.rigid_bodies.triangles.contact_surfaces.contact_surfaces_contact_point_tangent_1_impulse_offset
				);

				GLuint const contact_surfaces_contact_point_normal_0_target_velocity_index
				{
					glGetProgramResourceIndex(environment.state.old_triangle_contact_update_shader, GL_BUFFER_VARIABLE, "Contact_Surfaces.contact_surfaces[0].contact_point_normals[0].target_velocity")
				};
				glGetProgramResourceiv
				(
					environment.state.old_triangle_contact_update_shader, GL_BUFFER_VARIABLE, contact_surfaces_contact_point_normal_0_target_velocity_index,
					1u, &offset_label, 1u, nullptr, &environment.state.GPU_buffers.rigid_bodies.triangles.contact_surfaces.contact_surfaces_contact_point_normal_0_target_velocity_offset
				);

				GLuint const contact_surfaces_contact_point_normal_0_mass_index
				{
					glGetProgramResourceIndex(environment.state.old_triangle_contact_update_shader, GL_BUFFER_VARIABLE, "Contact_Surfaces.contact_surfaces[0].contact_point_normals[0].mass")
				};
				glGetProgramResourceiv
				(
					environment.state.old_triangle_contact_update_shader, GL_BUFFER_VARIABLE, contact_surfaces_contact_point_normal_0_mass_index,
					1u, &offset_label, 1u, nullptr, &environment.state.GPU_buffers.rigid_bodies.triangles.contact_surfaces.contact_surfaces_contact_point_normal_0_mass_offset
				);

				GLuint const contact_surfaces_contact_point_normal_0_impulse_index
				{
					glGetProgramResourceIndex(environment.state.old_triangle_contact_update_shader, GL_BUFFER_VARIABLE, "Contact_Surfaces.contact_surfaces[0].contact_point_normals[0].impulse")
				};
				glGetProgramResourceiv
				(
					environment.state.old_triangle_contact_update_shader, GL_BUFFER_VARIABLE, contact_surfaces_contact_point_normal_0_impulse_index,
					1u, &offset_label, 1u, nullptr, &environment.state.GPU_buffers.rigid_bodies.triangles.contact_surfaces.contact_surfaces_contact_point_normal_0_impulse_offset
				);

				GLuint const contact_surfaces_contact_point_normal_1_target_velocity_index
				{
					glGetProgramResourceIndex(environment.state.old_triangle_contact_update_shader, GL_BUFFER_VARIABLE, "Contact_Surfaces.contact_surfaces[0].contact_point_normals[1].target_velocity")
				};
				glGetProgramResourceiv
				(
					environment.state.old_triangle_contact_update_shader, GL_BUFFER_VARIABLE, contact_surfaces_contact_point_normal_1_target_velocity_index,
					1u, &offset_label, 1u, nullptr, &environment.state.GPU_buffers.rigid_bodies.triangles.contact_surfaces.contact_surfaces_contact_point_normal_1_target_velocity_offset
				);

				GLuint const contact_surfaces_contact_point_normal_1_mass_index
				{
					glGetProgramResourceIndex(environment.state.old_triangle_contact_update_shader, GL_BUFFER_VARIABLE, "Contact_Surfaces.contact_surfaces[0].contact_point_normals[1].mass")
				};
				glGetProgramResourceiv
				(
					environment.state.old_triangle_contact_update_shader, GL_BUFFER_VARIABLE, contact_surfaces_contact_point_normal_1_mass_index,
					1u, &offset_label, 1u, nullptr, &environment.state.GPU_buffers.rigid_bodies.triangles.contact_surfaces.contact_surfaces_contact_point_normal_1_mass_offset
				);

				GLuint const contact_surfaces_contact_point_normal_1_impulse_index
				{
					glGetProgramResourceIndex(environment.state.old_triangle_contact_update_shader, GL_BUFFER_VARIABLE, "Contact_Surfaces.contact_surfaces[0].contact_point_normals[1].impulse")
				};
				glGetProgramResourceiv
				(
					environment.state.old_triangle_contact_update_shader, GL_BUFFER_VARIABLE, contact_surfaces_contact_point_normal_1_impulse_index,
					1u, &offset_label, 1u, nullptr, &environment.state.GPU_buffers.rigid_bodies.triangles.contact_surfaces.contact_surfaces_contact_point_normal_1_impulse_offset
				);

				GLint offsets[]
				{
					environment.state.GPU_buffers.rigid_bodies.triangles.contact_surfaces.contact_surfaces_bodies_offset,
					environment.state.GPU_buffers.rigid_bodies.triangles.contact_surfaces.contact_surfaces_contact_point_position_0_offsets_offset,
					environment.state.GPU_buffers.rigid_bodies.triangles.contact_surfaces.contact_surfaces_contact_point_position_1_offsets_offset,
					environment.state.GPU_buffers.rigid_bodies.triangles.contact_surfaces.contact_surfaces_contact_point_tangent_0_mass_offset,
					environment.state.GPU_buffers.rigid_bodies.triangles.contact_surfaces.contact_surfaces_contact_point_tangent_0_impulse_offset,
					environment.state.GPU_buffers.rigid_bodies.triangles.contact_surfaces.contact_surfaces_contact_point_tangent_1_mass_offset,
					environment.state.GPU_buffers.rigid_bodies.triangles.contact_surfaces.contact_surfaces_contact_point_tangent_1_impulse_offset,
					environment.state.GPU_buffers.rigid_bodies.triangles.contact_surfaces.contact_surfaces_contact_point_normal_0_target_velocity_offset,
					environment.state.GPU_buffers.rigid_bodies.triangles.contact_surfaces.contact_surfaces_contact_point_normal_0_mass_offset,
					environment.state.GPU_buffers.rigid_bodies.triangles.contact_surfaces.contact_surfaces_contact_point_normal_0_impulse_offset,
					environment.state.GPU_buffers.rigid_bodies.triangles.contact_surfaces.contact_surfaces_contact_point_normal_1_target_velocity_offset,
					environment.state.GPU_buffers.rigid_bodies.triangles.contact_surfaces.contact_surfaces_contact_point_normal_1_mass_offset,
					environment.state.GPU_buffers.rigid_bodies.triangles.contact_surfaces.contact_surfaces_contact_point_normal_1_impulse_offset
				};
				environment.state.contact_surface_buffer_contact_surfaces_offset = *std::min_element(std::begin(offsets), std::end(offsets));
			}

#if USE_DYNAMIC_SIZES == true
			environment.state.GPU_buffers.rigid_bodies.triangles.contact_surfaces.size = environment.state.contact_surface_buffer_contact_surfaces_offset + game_logic_MAX_TRIANGLE_CONTACT_COUNT(environment) * environment.state.contact_surface_buffer_contact_surfaces_stride;
#else
			GLuint const block_index
			{
				glGetProgramResourceIndex(environment.state.old_triangle_contact_update_shader, GL_SHADER_STORAGE_BLOCK, "Contact_Surfaces")
			};
			GLenum const buffer_size_label{ GL_BUFFER_DATA_SIZE };
			glGetProgramResourceiv
			(
				environment.state.old_triangle_contact_update_shader, GL_SHADER_STORAGE_BLOCK, block_index,
				1, &buffer_size_label, 1, nullptr, &environment.state.GPU_buffers.rigid_bodies.triangles.contact_surfaces.size
			);
#endif

			glNamedBufferStorage
			(
				environment.state.contact_surface_buffer, environment.state.GPU_buffers.rigid_bodies.triangles.contact_surfaces.size, nullptr, 
				0u
			);

			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, game_logic__util_CONTACT_SURFACE_BINDING, environment.state.contact_surface_buffer);
		 }

		 { // Contact count buffer
			 GLuint const contact_count_index
			 {
				 glGetProgramResourceIndex(environment.state.old_triangle_contact_update_shader, GL_UNIFORM, "Contact_Count.contact_count")
			 };

			 GLenum offset_label{ GL_OFFSET };
			 glGetProgramResourceiv
			 (
				 environment.state.old_triangle_contact_update_shader, GL_UNIFORM, contact_count_index,
				 1u, &offset_label, 1u, nullptr, &environment.state.GPU_buffers.rigid_bodies.triangles.contact_count.contact_count_offset
			 );

			 GLuint const block_index
			 {
				 glGetProgramResourceIndex(environment.state.old_triangle_contact_update_shader, GL_UNIFORM_BLOCK, "Contact_Count")
			 };
			 GLenum const buffer_size_label{ GL_BUFFER_DATA_SIZE };
			 glGetProgramResourceiv
			 (
				 environment.state.old_triangle_contact_update_shader, GL_UNIFORM_BLOCK, block_index,
				 1u, &buffer_size_label, 1u, nullptr, &environment.state.GPU_buffers.rigid_bodies.triangles.contact_count.size
			 );

			 unsigned char* const initial_contact_count = new unsigned char[environment.state.GPU_buffers.rigid_bodies.triangles.contact_count.size];
			 std::memcpy(initial_contact_count + environment.state.GPU_buffers.rigid_bodies.triangles.contact_count.contact_count_offset, &environment.state.current_triangle_contact_count, sizeof(GLuint));

			 glNamedBufferStorage
			 (
				 environment.state.GPU_buffers.rigid_bodies.triangles.contact_count.buffer, environment.state.GPU_buffers.rigid_bodies.triangles.contact_count.size, initial_contact_count,
				 0u
			 );

			 delete[] initial_contact_count;

			 glBindBufferBase(GL_UNIFORM_BUFFER, game_logic__util_CONTACT_COUNT_BINDING, environment.state.GPU_buffers.rigid_bodies.triangles.contact_count.buffer);
		}

		{ // Persistent contact count buffer
			GLuint const persistent_contact_count_index
			{
				glGetProgramResourceIndex(environment.state.new_triangle_contact_shader, GL_UNIFORM, "Persistent_Contact_Count.persistent_contact_count")
			};

			GLenum offset_label{ GL_OFFSET };
			glGetProgramResourceiv
			(
				environment.state.new_triangle_contact_shader, GL_UNIFORM, persistent_contact_count_index,
				1u, &offset_label, 1u, nullptr, &environment.state.GPU_buffers.rigid_bodies.triangles.persistent_contact_count.persistent_contact_count_offset
			);

			GLuint const block_index
			{
				glGetProgramResourceIndex(environment.state.new_triangle_contact_shader, GL_UNIFORM_BLOCK, "Persistent_Contact_Count")
			};
			GLenum const buffer_size_label{ GL_BUFFER_DATA_SIZE };
			glGetProgramResourceiv
			(
				environment.state.new_triangle_contact_shader, GL_UNIFORM_BLOCK, block_index,
				1u, &buffer_size_label, 1u, nullptr, &environment.state.GPU_buffers.rigid_bodies.triangles.persistent_contact_count.size
			);

			unsigned char* const initial_persistent_contact_count = new unsigned char[environment.state.GPU_buffers.rigid_bodies.triangles.persistent_contact_count.size];
			std::memcpy(initial_persistent_contact_count + environment.state.GPU_buffers.rigid_bodies.triangles.persistent_contact_count.persistent_contact_count_offset, &environment.state.current_triangle_contact_count, sizeof(GLuint));

			glNamedBufferStorage
			(
				environment.state.GPU_buffers.rigid_bodies.triangles.persistent_contact_count.buffer, environment.state.GPU_buffers.rigid_bodies.triangles.persistent_contact_count.size, initial_persistent_contact_count,
				0u
			);

			delete[] initial_persistent_contact_count;

			glBindBufferBase(GL_UNIFORM_BUFFER, game_logic__util_PERSISTENT_CONTACT_COUNT_BINDING, environment.state.GPU_buffers.rigid_bodies.triangles.persistent_contact_count.buffer);
		}

		{ // Cursor position buffer
			GLuint const position_index
			{
				glGetProgramResourceIndex(environment.state.update_and_warm_start_cursor_constraint_shader, GL_UNIFORM, "Cursor_Position.position")
			};

			GLenum offset_label{ GL_OFFSET };
			glGetProgramResourceiv
			(
				environment.state.update_and_warm_start_cursor_constraint_shader, GL_UNIFORM, position_index,
				1u, &offset_label, 1u, nullptr, &environment.state.cursor_position_buffer_position_offset
			);

			GLuint const block_index
			{
				glGetProgramResourceIndex(environment.state.update_and_warm_start_cursor_constraint_shader, GL_UNIFORM_BLOCK, "Cursor_Position")
			};
			GLenum const buffer_size_label{ GL_BUFFER_DATA_SIZE };
			glGetProgramResourceiv
			(
				environment.state.update_and_warm_start_cursor_constraint_shader, GL_UNIFORM_BLOCK, block_index,
				1u, &buffer_size_label, 1u, nullptr, &environment.state.cursor_position_buffer_size
			);

			
			glNamedBufferStorage
			(
				environment.state.cursor_position_buffer, environment.state.cursor_position_buffer_size, nullptr,
				0u
			);

			glBindBufferBase(GL_UNIFORM_BUFFER, game_logic__util_CURSOR_POSITION_BINDING, environment.state.cursor_position_buffer);
		}

		{ // Cursor constrained point buffer
			GLenum offset_label{ GL_OFFSET };

			GLuint const body_index
			{
				glGetProgramResourceIndex(environment.state.update_and_warm_start_cursor_constraint_shader, GL_UNIFORM, "Cursor_Constrained_Point.body")
			};
			glGetProgramResourceiv
			(
				environment.state.update_and_warm_start_cursor_constraint_shader, GL_UNIFORM, body_index,
				1u, &offset_label, 1u, nullptr, &environment.state.cursor_constrained_point_buffer_body_offset
			);

			GLuint const local_point_index
			{
				glGetProgramResourceIndex(environment.state.update_and_warm_start_cursor_constraint_shader, GL_UNIFORM, "Cursor_Constrained_Point.local_point")
			};
			glGetProgramResourceiv
			(
				environment.state.update_and_warm_start_cursor_constraint_shader, GL_UNIFORM, local_point_index,
				1u, &offset_label, 1u, nullptr, &environment.state.cursor_constrained_point_buffer_local_point_offset
			);

			GLuint const block_index
			{
				glGetProgramResourceIndex(environment.state.update_and_warm_start_cursor_constraint_shader, GL_UNIFORM_BLOCK, "Cursor_Constrained_Point")
			};
			GLenum const buffer_size_label{ GL_BUFFER_DATA_SIZE };
			glGetProgramResourceiv
			(
				environment.state.update_and_warm_start_cursor_constraint_shader, GL_UNIFORM_BLOCK, block_index,
				1u, &buffer_size_label, 1u, nullptr, &environment.state.cursor_constrained_point_buffer_size
			);


			glNamedBufferStorage
			(
				environment.state.cursor_constrained_point_buffer, environment.state.cursor_constrained_point_buffer_size, nullptr,
				0u
			);

			glBindBufferBase(GL_UNIFORM_BUFFER, game_logic__util_CURSOR_CONSTRAINED_POINT_BINDING, environment.state.cursor_constrained_point_buffer);
		}

		{ // Cursor constraint buffer
			GLenum const offset_label{ GL_OFFSET };

			GLuint const offset_index
			{
				glGetProgramResourceIndex(environment.state.update_and_warm_start_cursor_constraint_shader, GL_BUFFER_VARIABLE, "Cursor_Constraint.offset")
			};
			glGetProgramResourceiv
			(
				environment.state.update_and_warm_start_cursor_constraint_shader, GL_BUFFER_VARIABLE, offset_index,
				1u, &offset_label, 1u, nullptr, &environment.state.cursor_constraint_buffer_offset_offset
			);

			GLuint const target_velocity_index
			{
				glGetProgramResourceIndex(environment.state.update_and_warm_start_cursor_constraint_shader, GL_BUFFER_VARIABLE, "Cursor_Constraint.target_velocity")
			};
			glGetProgramResourceiv
			(
				environment.state.update_and_warm_start_cursor_constraint_shader, GL_BUFFER_VARIABLE, target_velocity_index,
				1u, &offset_label, 1u, nullptr, &environment.state.cursor_constraint_buffer_target_velocity_offset
			);

			GLenum const prop_labels[]{ GL_OFFSET, GL_MATRIX_STRIDE };
			GLint props[2u];
			GLuint const mass_index
			{
				glGetProgramResourceIndex(environment.state.update_and_warm_start_cursor_constraint_shader, GL_BUFFER_VARIABLE, "Cursor_Constraint.mass")
			};
			glGetProgramResourceiv
			(
				environment.state.update_and_warm_start_cursor_constraint_shader, GL_BUFFER_VARIABLE, mass_index,
				std::size(prop_labels), prop_labels, 2u, nullptr, props
			);
			// TODO: Consider putting offset and matrix stride contigously in game state
			environment.state.cursor_constraint_buffer_mass_offset = props[0u];
			environment.state.cursor_constraint_buffer_mass_matrix_stride = props[1u];

			GLuint const impulse_index
			{
				glGetProgramResourceIndex(environment.state.update_and_warm_start_cursor_constraint_shader, GL_BUFFER_VARIABLE, "Cursor_Constraint.impulse")
			};
			glGetProgramResourceiv
			(
				environment.state.update_and_warm_start_cursor_constraint_shader, GL_BUFFER_VARIABLE, impulse_index,
				1u, &offset_label, 1u, nullptr, &environment.state.cursor_constraint_buffer_impulse_offset
			);

			GLuint const block_index
			{
				glGetProgramResourceIndex(environment.state.update_and_warm_start_cursor_constraint_shader, GL_SHADER_STORAGE_BLOCK, "Cursor_Constraint")
			};
			GLenum const buffer_size_label{ GL_BUFFER_DATA_SIZE };
			glGetProgramResourceiv
			(
				environment.state.update_and_warm_start_cursor_constraint_shader, GL_SHADER_STORAGE_BLOCK, block_index,
				1, &buffer_size_label, 1, nullptr, &environment.state.cursor_constraint_buffer_size
			);

			glNamedBufferStorage
			(
				environment.state.cursor_constraint_buffer, environment.state.cursor_constraint_buffer_size, nullptr,
				0u
			);

			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, game_logic__util_CURSOR_CONSTRAINT_BINDING, environment.state.cursor_constraint_buffer);
		}

		{ // Distance constraint buffer
			{
				GLenum const offset_label{ GL_OFFSET };

				GLuint const count_index
				{
					glGetProgramResourceIndex(environment.state.update_distance_constraints_shader, GL_BUFFER_VARIABLE, "Distance_Constraints.count")
				};
				glGetProgramResourceiv
				(
					environment.state.update_distance_constraints_shader, GL_BUFFER_VARIABLE, count_index,
					1u, &offset_label, 1u, nullptr, &environment.state.GPU_buffers.rigid_bodies.distance_constraints.count_offset
				);

				GLuint const distance_constraints_bodies_index
				{
					glGetProgramResourceIndex(environment.state.update_distance_constraints_shader, GL_BUFFER_VARIABLE, "Distance_Constraints.distance_constraints[0].bodies")
				};
				GLenum const prop_labels[]{ GL_OFFSET, GL_TOP_LEVEL_ARRAY_STRIDE };
				GLint props[std::size(prop_labels)];
				glGetProgramResourceiv
				(
					environment.state.update_distance_constraints_shader, GL_BUFFER_VARIABLE, distance_constraints_bodies_index,
					std::size(prop_labels), prop_labels, 2u, nullptr, props
				);
				// TODO: Consider putting offset and stride contigously in game state (but probably not)
				environment.state.GPU_buffers.rigid_bodies.distance_constraints.distance_constraints_bodies_offset = props[0u];
				environment.state.GPU_buffers.rigid_bodies.distance_constraints.distance_constraints_stride = props[1u];

				GLenum const array_prop_labels[]{ GL_OFFSET, GL_ARRAY_STRIDE };
				GLint array_props[std::size(array_prop_labels)];

				GLuint const distance_constraints_local_points_index
				{
					glGetProgramResourceIndex(environment.state.update_distance_constraints_shader, GL_BUFFER_VARIABLE, "Distance_Constraints.distance_constraints[0].local_points[0]")
				};
				glGetProgramResourceiv
				(
					environment.state.update_distance_constraints_shader, GL_BUFFER_VARIABLE, distance_constraints_local_points_index,
					std::size(array_prop_labels), array_prop_labels, 2u, nullptr, array_props
				);
				environment.state.GPU_buffers.rigid_bodies.distance_constraints.distance_constraints_local_points_offset = array_props[0u];
				environment.state.GPU_buffers.rigid_bodies.distance_constraints.distance_constraints_local_points_stride = array_props[1u];

				GLuint const distance_constraints_offsets_index
				{
					glGetProgramResourceIndex(environment.state.update_distance_constraints_shader, GL_BUFFER_VARIABLE, "Distance_Constraints.distance_constraints[0].offsets[0]")
				};
				glGetProgramResourceiv
				(
					environment.state.update_distance_constraints_shader, GL_BUFFER_VARIABLE, distance_constraints_offsets_index,
					std::size(array_prop_labels), array_prop_labels, 2u, nullptr, array_props
				);
				environment.state.GPU_buffers.rigid_bodies.distance_constraints.distance_constraints_offsets_offset = array_props[0u];
				environment.state.GPU_buffers.rigid_bodies.distance_constraints.distance_constraints_offsets_stride = array_props[1u];

				GLuint const distance_constraints_direction_index
				{
					glGetProgramResourceIndex(environment.state.update_distance_constraints_shader, GL_BUFFER_VARIABLE, "Distance_Constraints.distance_constraints[0].direction")
				};
				glGetProgramResourceiv
				(
					environment.state.update_distance_constraints_shader, GL_BUFFER_VARIABLE, distance_constraints_direction_index,
					1u, &offset_label, 1u, nullptr, &environment.state.GPU_buffers.rigid_bodies.distance_constraints.distance_constraints_direction_offset
				);

				GLuint const distance_constraints_max_distance_index
				{
					glGetProgramResourceIndex(environment.state.update_distance_constraints_shader, GL_BUFFER_VARIABLE, "Distance_Constraints.distance_constraints[0].max_distance")
				};
				glGetProgramResourceiv
				(
					environment.state.update_distance_constraints_shader, GL_BUFFER_VARIABLE, distance_constraints_max_distance_index,
					1u, &offset_label, 1u, nullptr, &environment.state.GPU_buffers.rigid_bodies.distance_constraints.distance_constraints_max_distance_offset
				);

				GLuint const distance_constraints_target_velocity_index
				{
					glGetProgramResourceIndex(environment.state.update_distance_constraints_shader, GL_BUFFER_VARIABLE, "Distance_Constraints.distance_constraints[0].target_velocity")
				};
				glGetProgramResourceiv
				(
					environment.state.update_distance_constraints_shader, GL_BUFFER_VARIABLE, distance_constraints_target_velocity_index,
					1u, &offset_label, 1u, nullptr, &environment.state.GPU_buffers.rigid_bodies.distance_constraints.distance_constraints_target_velocity_offset
				);

				GLuint const distance_constraints_mass_index
				{
					glGetProgramResourceIndex(environment.state.update_distance_constraints_shader, GL_BUFFER_VARIABLE, "Distance_Constraints.distance_constraints[0].mass")
				};
				glGetProgramResourceiv
				(
					environment.state.update_distance_constraints_shader, GL_BUFFER_VARIABLE, distance_constraints_mass_index,
					1u, &offset_label, 1u, nullptr, &environment.state.GPU_buffers.rigid_bodies.distance_constraints.distance_constraints_mass_offset
				);

				GLuint const distance_constraints_impulse_index
				{
					glGetProgramResourceIndex(environment.state.update_distance_constraints_shader, GL_BUFFER_VARIABLE, "Distance_Constraints.distance_constraints[0].impulse")
				};
				glGetProgramResourceiv
				(
					environment.state.update_distance_constraints_shader, GL_BUFFER_VARIABLE, distance_constraints_impulse_index,
					1u, &offset_label, 1u, nullptr, &environment.state.GPU_buffers.rigid_bodies.distance_constraints.distance_constraints_impulse_offset
				);

				GLint offsets[]
				{
					environment.state.GPU_buffers.rigid_bodies.distance_constraints.distance_constraints_bodies_offset,
					environment.state.GPU_buffers.rigid_bodies.distance_constraints.distance_constraints_local_points_offset,
					environment.state.GPU_buffers.rigid_bodies.distance_constraints.distance_constraints_offsets_offset,
					environment.state.GPU_buffers.rigid_bodies.distance_constraints.distance_constraints_direction_offset,
					environment.state.GPU_buffers.rigid_bodies.distance_constraints.distance_constraints_max_distance_offset,
					environment.state.GPU_buffers.rigid_bodies.distance_constraints.distance_constraints_target_velocity_offset,
					environment.state.GPU_buffers.rigid_bodies.distance_constraints.distance_constraints_mass_offset,
					environment.state.GPU_buffers.rigid_bodies.distance_constraints.distance_constraints_impulse_offset,
				};
				environment.state.GPU_buffers.rigid_bodies.distance_constraints.distance_constraints_offset = *std::min_element(std::begin(offsets), std::end(offsets));
			}

#if USE_DYNAMIC_SIZES == true
			environment.state.GPU_buffers.rigid_bodies.distance_constraints.size = environment.state.GPU_buffers.rigid_bodies.distance_constraints.distance_constraints_offset + game_logic_MAX_DISTANCE_CONSTRAINT_COUNT(environment) * environment.state.GPU_buffers.rigid_bodies.distance_constraints.distance_constraints_stride;
#else
			GLuint const block_index
			{
				glGetProgramResourceIndex(environment.state.update_distance_constraints_shader, GL_SHADER_STORAGE_BLOCK, "Distance_Constraints")
			};
			GLenum const buffer_size_label{ GL_BUFFER_DATA_SIZE };
			glGetProgramResourceiv
			(
				environment.state.update_distance_constraints_shader, GL_SHADER_STORAGE_BLOCK, block_index,
				1u, &buffer_size_label, 1u, nullptr, &environment.state.distance_constraint_buffer_size
			);
#endif
			// IMPORTANT TODO: REMOVE FLAGS!!!
			glNamedBufferStorage
			(
				environment.state.GPU_buffers.rigid_bodies.distance_constraints.buffer, environment.state.GPU_buffers.rigid_bodies.distance_constraints.size, nullptr,
				GL_MAP_PERSISTENT_BIT | GL_MAP_READ_BIT | GL_MAP_WRITE_BIT
			);
			
			glClearNamedBufferSubData
			(
				environment.state.GPU_buffers.rigid_bodies.distance_constraints.buffer,
				GL_R32UI, 
				environment.state.GPU_buffers.rigid_bodies.distance_constraints.count_offset, sizeof(GLuint),
				GL_RED, GL_UNSIGNED_INT, 
				&environment.state.current_distance_constraint_count
			);

			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, game_logic__util_DISTANCE_CONSTRAINT_BINDING, environment.state.GPU_buffers.rigid_bodies.distance_constraints.buffer);
		}

//		{
//			GLuint const p_index
//			{
//				glGetProgramResourceIndex(environment.state.integrate_fluid_velocity_shader, GL_BUFFER_VARIABLE, "Fluid_Position.p")
//			};
//
//			GLenum const prop_labels[]{ GL_OFFSET, GL_ARRAY_STRIDE };
//			GLint props[std::size(prop_labels)];
//			glGetProgramResourceiv
//			(
//				environment.state.integrate_fluid_velocity_shader, GL_BUFFER_VARIABLE, p_index,
//				std::size(prop_labels), prop_labels, 2, nullptr, props
//			);
//			// TODO: Consider putting p_offset and p_stride contigously in game state
//			environment.state.fluid_position_buffer_p_offset = props[0u];
//			environment.state.fluid_position_buffer_p_stride = props[1u];
//
//#if USE_DYNAMIC_SIZES == true
//			environment.state.fluid_position_buffer_size = environment.state.fluid_position_buffer_p_offset + game_logic_MAX_FLUID_PARTICLE_COUNT(environment) * environment.state.fluid_position_buffer_p_stride;
//#else
//			GLuint const block_index
//			{
//				glGetProgramResourceIndex(environment.state.integrate_fluid_velocity_shader, GL_SHADER_STORAGE_BLOCK, "Fluid_Position")
//			};
//			GLenum const buffer_size_label{ GL_BUFFER_DATA_SIZE };
//			glGetProgramResourceiv
//			(
//				environment.state.integrate_fluid_velocity_shader, GL_SHADER_STORAGE_BLOCK, block_index,
//				1, &buffer_size_label, 1, nullptr, &environment.state.fluid_position_buffer_size
//			);
//#endif
//
//			// TODO: Don't initialize a few positions by copying over the ENTIRE buffer 
//			// content from CPU to GPU like this. Instead, use persistent mapping 
//			// for both initialization and updating.
//			unsigned char* const initial_fluid_position = new unsigned char[environment.state.fluid_position_buffer_size];
//
//			for (GLuint i = 0; i < environment.state.current_fluid_particle_count; ++i)
//			{
//				GLuint const width{ 10u };
//
//				::util::math::Vector_2D position
//				{
//					-game_logic__util__spatial_FROM_METERS(environment, (i % width) * 5.0f),
//					-game_logic__util__spatial_FROM_METERS(environment, (i / width) * 1.2f)
//				};
//
//				std::memcpy
//				(
//					initial_fluid_position + environment.state.fluid_position_buffer_p_offset + i * environment.state.fluid_position_buffer_p_stride,
//					&position, sizeof(position)
//				);
//			}
//
//			glNamedBufferStorage
//			(
//				environment.state.fluid_position_buffer, environment.state.fluid_position_buffer_size, initial_fluid_position,
//				0u//GL_MAP_PERSISTENT_BIT | GL_MAP_READ_BIT | GL_MAP_WRITE_BIT //0u
//			);
//
//			delete[] initial_fluid_position;
//
//			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, game_logic__util_FLUID_POSITION_BINDING, environment.state.fluid_position_buffer);
//
//			/*environment.state.position_mapping = static_cast<unsigned char*>
//			(
//				glMapNamedBufferRange
//				(
//					environment.state.rigid_body_position_buffer,
//					0u, environment.state.rigid_body_position_buffer_size,
//					GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_FLUSH_EXPLICIT_BIT
//				)
//			);*/
//		}
//
//		{ // Fluid velocity buffer
//			GLuint const v_index
//			{
//				glGetProgramResourceIndex(environment.state.integrate_fluid_velocity_shader, GL_BUFFER_VARIABLE, "Fluid_Velocity.v")
//			};
//
//			GLenum const prop_labels[]{ GL_OFFSET, GL_ARRAY_STRIDE };
//			GLint props[std::size(prop_labels)];
//			glGetProgramResourceiv
//			(
//				environment.state.integrate_fluid_velocity_shader, GL_BUFFER_VARIABLE, v_index,
//				std::size(prop_labels), prop_labels, 2, nullptr, props
//			);
//			// TODO: Consider putting v_offset and v_stride contigously in game state
//			environment.state.fluid_velocity_buffer_v_offset = props[0u];
//			environment.state.fluid_velocity_buffer_v_stride = props[1u];
//
//#if USE_DYNAMIC_SIZES == true
//			environment.state.fluid_velocity_buffer_size = environment.state.fluid_velocity_buffer_v_offset + game_logic_MAX_FLUID_PARTICLE_COUNT(environment) * environment.state.fluid_velocity_buffer_v_stride;
//#else
//			GLuint const block_index
//			{
//				glGetProgramResourceIndex(environment.state.integrate_fluid_velocity_shader, GL_SHADER_STORAGE_BLOCK, "Fluid_Velocity")
//			};
//			GLenum const buffer_size_label{ GL_BUFFER_DATA_SIZE };
//			glGetProgramResourceiv
//			(
//				environment.state.integrate_fluid_velocity_shader, GL_SHADER_STORAGE_BLOCK, block_index,
//				1, &buffer_size_label, 1, nullptr, &environment.state.fluid_velocity_buffer_size
//			);
//#endif
//
//			// TODO: Don't initialize a few velocities by copying over the ENTIRE buffer 
//			// content from CPU to GPU like this. Instead, use persistent mapping 
//			// for both initialization and updating.
//			unsigned char* const initial_fluid_velocity = new unsigned char[environment.state.fluid_velocity_buffer_size];
//
//			for (GLuint i = 0; i < environment.state.current_fluid_particle_count; ++i)
//			{
//				//GLuint const width{ 316u };
//
//				::util::math::Vector_2D velocity
//				{
//					game_METERS_PER_SECOND_TO_LENGTH_PER_TICK(environment, 0.0f),
//					game_METERS_PER_SECOND_TO_LENGTH_PER_TICK(environment, 0.0f)
//				};
//				if (i % 2u == 0u)
//				{
//					velocity.x = game_METERS_PER_SECOND_TO_LENGTH_PER_TICK(environment, -10.0f);
//				}
//
//				std::memcpy
//				(
//					initial_fluid_velocity + environment.state.fluid_velocity_buffer_v_offset + i * environment.state.fluid_velocity_buffer_v_stride,
//					&velocity, sizeof(velocity)
//				);
//			}
//
//			glNamedBufferStorage
//			(
//				environment.state.fluid_velocity_buffer, environment.state.fluid_velocity_buffer_size, initial_fluid_velocity,
//				0u//GL_MAP_PERSISTENT_BIT | GL_MAP_READ_BIT | GL_MAP_WRITE_BIT
//			);
//
//			delete[] initial_fluid_velocity;
//
//			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, game_logic__util_FLUID_VELOCITY_BINDING, environment.state.fluid_velocity_buffer);
//
//			/*environment.state.position_mapping = static_cast<unsigned char*>
//			(
//				glMapNamedBufferRange
//				(
//					environment.state.rigid_body_position_buffer,
//					0u, environment.state.rigid_body_position_buffer_size,
//					GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_FLUSH_EXPLICIT_BIT
//				)
//			);*/
//		}
//
//		{ // Fluid bounding box buffer
//			GLuint const boxes_index
//			{
//				glGetProgramResourceIndex(environment.state.integrate_fluid_velocity_shader, GL_BUFFER_VARIABLE, "Fluid_Bounding_Boxes.boxes")
//			};
//
//			GLenum const prop_labels[]{ GL_OFFSET, GL_ARRAY_STRIDE };
//			GLint props[std::size(prop_labels)];
//			glGetProgramResourceiv
//			(
//				environment.state.integrate_fluid_velocity_shader, GL_BUFFER_VARIABLE, boxes_index,
//				std::size(prop_labels), prop_labels, 2u, nullptr, props
//			);
//			// TODO: Consider putting offset and stride contigously in game state
//			environment.state.fluid_bounding_box_buffer_boxes_offset = props[0u];
//			environment.state.fluid_bounding_box_buffer_boxes_stride = props[1u];
//
//#if USE_DYNAMIC_SIZES == true
//			environment.state.fluid_bounding_box_buffer_size = environment.state.fluid_bounding_box_buffer_boxes_offset + game_logic_MAX_FLUID_PARTICLE_COUNT(environment) * environment.state.fluid_bounding_box_buffer_boxes_stride;
//#else
//			GLuint const block_index
//			{
//				glGetProgramResourceIndex(environment.state.integrate_fluid_velocity_shader, GL_SHADER_STORAGE_BLOCK, "Fluid_Bounding_Boxes")
//			};
//			GLenum const buffer_size_label{ GL_BUFFER_DATA_SIZE };
//			glGetProgramResourceiv
//			(
//				environment.state.integrate_fluid_velocity_shader, GL_SHADER_STORAGE_BLOCK, block_index,
//				1u, &buffer_size_label, 1u, nullptr, &environment.state.fluid_bounding_box_buffer_size
//			);
//#endif
//
//			// TODO: Don't initialize a few boxes by copying over the ENTIRE buffer 
//			// content from CPU to GPU like this. Instead, use persistent mapping 
//			// for both initialization and updating.
//			unsigned char* const initial_boxes = new unsigned char[environment.state.fluid_bounding_box_buffer_size];
//
//			util::rigid_body::Triangle_Bounding_Box box
//			{
//				{
//					0,
//					0
//				},
//				{
//					-1,
//					-1
//				}
//			};
//			for (GLuint i = 0u; i < environment.state.current_fluid_particle_count; ++i)
//			{
//				std::memcpy
//				(
//					initial_boxes + environment.state.fluid_bounding_box_buffer_boxes_offset + i * environment.state.fluid_bounding_box_buffer_boxes_stride,
//					&box, sizeof(box)
//				);
//			}
//
//			glNamedBufferStorage
//			(
//				environment.state.fluid_bounding_box_buffer, environment.state.fluid_bounding_box_buffer_size, initial_boxes,
//				0u
//			);
//
//			delete[] initial_boxes;
//
//			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, game_logic__util_FLUID_BOUNDING_BOX_BINDING, environment.state.fluid_bounding_box_buffer);
//		}
//
//		{ // Changed fluid bounding box buffer
//			{
//				GLuint const size_index
//				{
//					glGetProgramResourceIndex(environment.state.integrate_fluid_velocity_shader, GL_BUFFER_VARIABLE, "Changed_Fluid_Bounding_Boxes.size")
//				};
//				GLenum const offset_label{ GL_OFFSET };
//				glGetProgramResourceiv
//				(
//					environment.state.integrate_fluid_velocity_shader, GL_BUFFER_VARIABLE, size_index,
//					1u, &offset_label, 1u, nullptr, &environment.state.GPU_buffers.fluid.changed_bounding_boxes.size_offset
//				);
//			}
//
//			{
//				GLuint const boxes_index_index
//				{
//					glGetProgramResourceIndex(environment.state.integrate_fluid_velocity_shader, GL_BUFFER_VARIABLE, "Changed_Fluid_Bounding_Boxes.boxes[0].index")
//				};
//				GLenum const prop_labels[]{ GL_OFFSET, GL_TOP_LEVEL_ARRAY_STRIDE };
//				GLint props[std::size(prop_labels)];
//				glGetProgramResourceiv
//				(
//					environment.state.integrate_fluid_velocity_shader, GL_BUFFER_VARIABLE, boxes_index_index,
//					std::size(prop_labels), prop_labels, 2u, nullptr, props
//				);
//				// TODO: Consider putting offset and stride contigously in game state
//				environment.state.GPU_buffers.fluid.changed_bounding_boxes.boxes_index_offset = props[0u];
//				environment.state.GPU_buffers.fluid.changed_bounding_boxes.boxes_stride = props[1u];
//
//				GLenum const offset_label{ GL_OFFSET };
//
//				GLuint const boxes_min_x_index
//				{
//					glGetProgramResourceIndex(environment.state.integrate_fluid_velocity_shader, GL_BUFFER_VARIABLE, "Changed_Fluid_Bounding_Boxes.boxes[0].min_x")
//				};
//				glGetProgramResourceiv
//				(
//					environment.state.integrate_fluid_velocity_shader, GL_BUFFER_VARIABLE, boxes_min_x_index,
//					1u, &offset_label, 1u, nullptr, &environment.state.GPU_buffers.fluid.changed_bounding_boxes.boxes_min_x_offset
//				);
//
//				GLuint const boxes_min_y_index
//				{
//					glGetProgramResourceIndex(environment.state.integrate_fluid_velocity_shader, GL_BUFFER_VARIABLE, "Changed_Fluid_Bounding_Boxes.boxes[0].min_y")
//				};
//				glGetProgramResourceiv
//				(
//					environment.state.integrate_fluid_velocity_shader, GL_BUFFER_VARIABLE, boxes_min_y_index,
//					1u, &offset_label, 1u, nullptr, &environment.state.GPU_buffers.fluid.changed_bounding_boxes.boxes_min_y_offset
//				);
//
//				GLuint const boxes_max_x_index
//				{
//					glGetProgramResourceIndex(environment.state.integrate_fluid_velocity_shader, GL_BUFFER_VARIABLE, "Changed_Fluid_Bounding_Boxes.boxes[0].max_x")
//				};
//				glGetProgramResourceiv
//				(
//					environment.state.integrate_fluid_velocity_shader, GL_BUFFER_VARIABLE, boxes_max_x_index,
//					1u, &offset_label, 1u, nullptr, &environment.state.GPU_buffers.fluid.changed_bounding_boxes.boxes_max_x_offset
//				);
//
//				GLuint const boxes_max_y_index
//				{
//					glGetProgramResourceIndex(environment.state.integrate_fluid_velocity_shader, GL_BUFFER_VARIABLE, "Changed_Fluid_Bounding_Boxes.boxes[0].max_y")
//				};
//				glGetProgramResourceiv
//				(
//					environment.state.integrate_fluid_velocity_shader, GL_BUFFER_VARIABLE, boxes_max_y_index,
//					1u, &offset_label, 1u, nullptr, &environment.state.GPU_buffers.fluid.changed_bounding_boxes.boxes_max_y_offset
//				);
//			}
//
//#if USE_DYNAMIC_SIZES == true
//			GLint const offsets[]
//			{
//			   environment.state.GPU_buffers.fluid.changed_bounding_boxes.boxes_index_offset,
//			   environment.state.GPU_buffers.fluid.changed_bounding_boxes.boxes_min_x_offset,
//			   environment.state.GPU_buffers.fluid.changed_bounding_boxes.boxes_min_y_offset,
//			   environment.state.GPU_buffers.fluid.changed_bounding_boxes.boxes_max_x_offset,
//			   environment.state.GPU_buffers.fluid.changed_bounding_boxes.boxes_max_y_offset
//			};
//			GLint const offset{ *std::min_element(std::begin(offsets), std::end(offsets)) };
//			environment.state.GPU_buffers.fluid.changed_bounding_boxes.size = offset + game_logic_MAX_FLUID_PARTICLE_COUNT(environment) * environment.state.GPU_buffers.fluid.changed_bounding_boxes.boxes_stride;
//#else
//			GLuint const block_index
//			{
//				glGetProgramResourceIndex(environment.state.integrate_fluid_velocity_shader, GL_SHADER_STORAGE_BLOCK, "Changed_Fluid_Bounding_Boxes")
//			};
//			GLenum const buffer_size_label{ GL_BUFFER_DATA_SIZE };
//			glGetProgramResourceiv
//			(
//				environment.state.integrate_fluid_velocity_shader, GL_SHADER_STORAGE_BLOCK, block_index,
//				1u, &buffer_size_label, 1u, nullptr, &environment.state.GPU_buffers.fluid.changed_bounding_boxes.size
//			);
//#endif
//
//			glNamedBufferStorage
//			(
//				environment.state.GPU_buffers.fluid.changed_bounding_boxes.buffer, environment.state.GPU_buffers.fluid.changed_bounding_boxes.size, nullptr,
//				GL_MAP_READ_BIT | GL_MAP_PERSISTENT_BIT
//			);
//			glClearNamedBufferSubData
//			(
//				environment.state.GPU_buffers.fluid.changed_bounding_boxes.buffer,
//				GL_R32UI,
//				environment.state.GPU_buffers.fluid.changed_bounding_boxes.size_offset, sizeof(GLuint),
//				GL_RED_INTEGER, GL_UNSIGNED_INT,
//				nullptr
//			);
//
//			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, game_logic__util_CHANGED_FLUID_BOUNDING_BOX_BINDING, environment.state.GPU_buffers.fluid.changed_bounding_boxes.buffer);
//
//			environment.state.changed_fluid_bounding_boxes_mapping = static_cast<unsigned char*>
//			(
//				glMapNamedBufferRange
//				(
//					environment.state.GPU_buffers.fluid.changed_bounding_boxes.buffer,
//					0u, environment.state.GPU_buffers.fluid.changed_bounding_boxes.size,
//					GL_MAP_READ_BIT | GL_MAP_PERSISTENT_BIT
//				)
//			);
//		}
//
//		{ // Fluid contact buffer
//			{
//				GLuint const contacts_inactive_index
//				{
//					glGetProgramResourceIndex(environment.state.persist_fluid_contacts_shader, GL_BUFFER_VARIABLE, "Fluid_Contacts.contacts[0].inactive")
//				};
//				GLenum const prop_labels[]{ GL_OFFSET, GL_TOP_LEVEL_ARRAY_STRIDE };
//				GLint props[std::size(prop_labels)];
//				glGetProgramResourceiv
//				(
//					environment.state.persist_fluid_contacts_shader, GL_BUFFER_VARIABLE, contacts_inactive_index,
//					std::size(prop_labels), prop_labels, 2u, nullptr, props
//				);
//				// TODO: Consider putting offset and stride contigously in game state
//				environment.state.fluid_contact_buffer_contacts_inactive_offset = props[0u];
//				environment.state.fluid_contact_buffer_contacts_stride = props[1u];
//
//				GLenum const offset_label{ GL_OFFSET };
//
//				GLuint const impulse_index
//				{
//					glGetProgramResourceIndex(environment.state.persist_fluid_contacts_shader, GL_BUFFER_VARIABLE, "Fluid_Contacts.contacts[0].impulse")
//				};
//				glGetProgramResourceiv
//				(
//					environment.state.persist_fluid_contacts_shader, GL_BUFFER_VARIABLE, impulse_index,
//					1u, &offset_label, 1u, nullptr, &environment.state.fluid_contact_buffer_contacts_impulse_offset
//				);
//
//				GLuint const contacts_particles_index
//				{
//					glGetProgramResourceIndex(environment.state.persist_fluid_contacts_shader, GL_BUFFER_VARIABLE, "Fluid_Contacts.contacts[0].particles")
//				};
//				glGetProgramResourceiv
//				(
//					environment.state.persist_fluid_contacts_shader, GL_BUFFER_VARIABLE, contacts_particles_index,
//					1u, &offset_label, 1u, nullptr, &environment.state.fluid_contact_buffer_contacts_particles_offset
//				);
//
//				GLuint const direction_index
//				{
//					glGetProgramResourceIndex(environment.state.persist_fluid_contacts_shader, GL_BUFFER_VARIABLE, "Fluid_Contacts.contacts[0].direction")
//				};
//				glGetProgramResourceiv
//				(
//					environment.state.persist_fluid_contacts_shader, GL_BUFFER_VARIABLE, direction_index,
//					1u, &offset_label, 1u, nullptr, &environment.state.fluid_contact_buffer_contacts_direction_offset
//				);
//
//				GLuint const target_velocity_index
//				{
//					glGetProgramResourceIndex(environment.state.persist_fluid_contacts_shader, GL_BUFFER_VARIABLE, "Fluid_Contacts.contacts[0].target_velocity")
//				};
//				glGetProgramResourceiv
//				(
//					environment.state.persist_fluid_contacts_shader, GL_BUFFER_VARIABLE, target_velocity_index,
//					1u, &offset_label, 1u, nullptr, &environment.state.fluid_contact_buffer_contacts_target_velocity_offset
//				);
//
//				GLuint const mass_index
//				{
//					glGetProgramResourceIndex(environment.state.persist_fluid_contacts_shader, GL_BUFFER_VARIABLE, "Fluid_Contacts.contacts[0].mass")
//				};
//				glGetProgramResourceiv
//				(
//					environment.state.persist_fluid_contacts_shader, GL_BUFFER_VARIABLE, mass_index,
//					1u, &offset_label, 1u, nullptr, &environment.state.fluid_contact_buffer_contacts_mass_offset
//				);
//
//				GLuint const impulse_range_index
//				{
//					glGetProgramResourceIndex(environment.state.persist_fluid_contacts_shader, GL_BUFFER_VARIABLE, "Fluid_Contacts.contacts[0].impulse_range")
//				};
//				glGetProgramResourceiv
//				(
//					environment.state.persist_fluid_contacts_shader, GL_BUFFER_VARIABLE, impulse_range_index,
//					1u, &offset_label, 1u, nullptr, &environment.state.fluid_contact_buffer_contacts_impulse_range_offset
//				);			
//			}
//
//			GLint const offsets[]
//			{
//			   environment.state.fluid_contact_buffer_contacts_inactive_offset,
//			   environment.state.fluid_contact_buffer_contacts_impulse_offset,
//			   environment.state.fluid_contact_buffer_contacts_particles_offset,
//			   environment.state.fluid_contact_buffer_contacts_direction_offset,
//			   environment.state.fluid_contact_buffer_contacts_target_velocity_offset,
//			   environment.state.fluid_contact_buffer_contacts_mass_offset,
//			   environment.state.fluid_contact_buffer_contacts_impulse_range_offset
//			};
//			environment.state.fluid_contact_buffer_contacts_offset = *std::min_element(std::begin(offsets), std::end(offsets));
//#if USE_DYNAMIC_SIZES == true
//			environment.state.GPU_buffers.fluid.contacts.size = environment.state.fluid_contact_buffer_contacts_offset + MAX_FLUID_CONTACT_COUNT(environment) * environment.state.fluid_contact_buffer_contacts_stride;
//#else
//			GLuint const block_index
//			{
//				glGetProgramResourceIndex(environment.state.persist_fluid_contacts_shader, GL_SHADER_STORAGE_BLOCK, "Fluid_Contacts")
//			};
//			GLenum const buffer_size_label{ GL_BUFFER_DATA_SIZE };
//			glGetProgramResourceiv
//			(
//				environment.state.GPU_buffers.fluid.contacts.size, GL_SHADER_STORAGE_BLOCK, block_index,
//				1u, &buffer_size_label, 1u, nullptr, &environment.state.GPU_buffers.fluid.contacts.size
//			);
//#endif
//
//			glNamedBufferStorage
//			(
//				environment.state.GPU_buffers.fluid.contacts.buffer, environment.state.GPU_buffers.fluid.contacts.size, nullptr,
//				GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT
//			);
//
//			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, game_logic__util_FLUID_CONTACT_BINDING, environment.state.fluid_contact_buffer);
//
//			environment.state.fluid_contact_mapping = static_cast<unsigned char*>
//			(
//				glMapNamedBufferRange
//				(
//					environment.state.GPU_buffers.fluid.contacts.buffer,
//					0u, environment.state.GPU_buffers.fluid.contacts.size,
//					GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT | GL_MAP_FLUSH_EXPLICIT_BIT
//				)
//			);
//		}
//
//		{ // Fluid contact count buffer
//			GLenum offset_label{ GL_OFFSET };
//
//			/*GLuint const persistent_count_index
//			{
//				glGetProgramResourceIndex(environment.state.persist_fluid_contacts_shader, GL_UNIFORM, "Fluid_Contact_Count.persistent_count")
//			};
//			glGetProgramResourceiv
//			(
//				environment.state.persist_fluid_contacts_shader, GL_UNIFORM, persistent_count_index,
//				1u, &offset_label, 1u, nullptr, &environment.state.fluid_contact_count_buffer_persistent_count_offset
//			);*/
//			
//			GLuint const count_index
//			{
//				glGetProgramResourceIndex(environment.state.persist_fluid_contacts_shader, GL_UNIFORM, "Fluid_Contact_Count.count")
//			};
//			glGetProgramResourceiv
//			(
//				environment.state.persist_fluid_contacts_shader, GL_UNIFORM, count_index,
//				1u, &offset_label, 1u, nullptr, &environment.state.fluid_contact_count_buffer_count_offset
//			);
//
//			GLuint const block_index
//			{
//				glGetProgramResourceIndex(environment.state.persist_fluid_contacts_shader, GL_UNIFORM_BLOCK, "Fluid_Contact_Count")
//			};
//			GLenum const buffer_size_label{ GL_BUFFER_DATA_SIZE };
//			glGetProgramResourceiv
//			(
//				environment.state.persist_fluid_contacts_shader, GL_UNIFORM_BLOCK, block_index,
//				1u, &buffer_size_label, 1u, nullptr, &environment.state.fluid_contact_count_buffer_size
//			);
//
//			unsigned char* const initial_fluid_contact_count = new unsigned char[environment.state.fluid_contact_count_buffer_size];
//			//std::memcpy(initial_fluid_contact_count + environment.state.fluid_contact_count_buffer_persistent_count_offset, &environment.state.current_fluid_contact_count, sizeof(GLuint));
//			std::memcpy(initial_fluid_contact_count + environment.state.fluid_contact_count_buffer_count_offset, &environment.state.current_fluid_contact_count, sizeof(GLuint));
//
//			glNamedBufferStorage
//			(
//				environment.state.fluid_contact_count_buffer, environment.state.fluid_contact_count_buffer_size, initial_fluid_contact_count,
//				0u
//			);
//
//			delete[] initial_fluid_contact_count;
//
//			glBindBufferBase(GL_UNIFORM_BUFFER, game_logic__util_FLUID_CONTACT_COUNT_BINDING, environment.state.fluid_contact_count_buffer);
//		}
//
//
//		{ // Fluid velocity snapshot buffer
//			glNamedBufferStorage
//			(
//				environment.state.fluid_velocity_snapshot_buffer, environment.state.fluid_velocity_buffer_size, nullptr,
//				0u
//			);
//			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, game_logic__util_FLUID_VELOCITY_SNAPSHOT_BINDING, environment.state.fluid_velocity_snapshot_buffer);
//		}

		{ // Gravity sources buffer
			GLuint const block_index
			{
				glGetProgramResourceIndex(environment.state.gravity_directions_draw_shader, GL_UNIFORM_BLOCK, "Gravity_Sources")
			};
			GLenum const buffer_size_label{ GL_BUFFER_DATA_SIZE };
			glGetProgramResourceiv
			(
				environment.state.gravity_directions_draw_shader, GL_UNIFORM_BLOCK, block_index,
				1u, &buffer_size_label, 1u, nullptr, &environment.state.gravity_sources_buffer_size
			);

			GLenum const offset_label{ GL_OFFSET };

			GLuint const count_index
			{
				glGetProgramResourceIndex(environment.state.gravity_directions_draw_shader, GL_UNIFORM, "Gravity_Sources.count")
			};
			glGetProgramResourceiv
			(
				environment.state.gravity_directions_draw_shader, GL_UNIFORM, count_index,
				1u, &offset_label, 1u, nullptr, &environment.state.gravity_sources_buffer_count_offset
			);

			GLenum prop_labels[]{ GL_OFFSET, GL_ARRAY_STRIDE };
			GLint props[2u];

			GLuint const positions_index
			{
				glGetProgramResourceIndex(environment.state.gravity_directions_draw_shader, GL_UNIFORM, "Gravity_Sources.positions")
			};
			glGetProgramResourceiv
			(
				environment.state.gravity_directions_draw_shader, GL_UNIFORM, positions_index,
				std::size(prop_labels), prop_labels, 2u, nullptr, props
			);
			environment.state.gravity_sources_buffer_positions_offset = props[0u];
			environment.state.gravity_sources_buffer_positions_stride = props[1u];

			GLuint const strengths_index
			{
				glGetProgramResourceIndex(environment.state.gravity_directions_draw_shader, GL_UNIFORM, "Gravity_Sources.strengths")
			};
			glGetProgramResourceiv
			(
				environment.state.gravity_directions_draw_shader, GL_UNIFORM, strengths_index,
				std::size(prop_labels), prop_labels, 2u, nullptr, props
			);
			environment.state.gravity_sources_buffer_strengths_offset = props[0u];
			environment.state.gravity_sources_buffer_strengths_stride = props[1u];

			glNamedBufferStorage
			(
				environment.state.gravity_sources_buffer, environment.state.gravity_sources_buffer_size, nullptr,
				0u
			);
			glClearNamedBufferSubData
			(
				environment.state.gravity_sources_buffer,
				GL_R32UI,
				environment.state.gravity_sources_buffer_count_offset, sizeof(GLuint),
				GL_RED_INTEGER, GL_UNSIGNED_INT,
				&environment.state.current_gravity_source_count
			);
			glBindBufferBase(GL_UNIFORM_BUFFER, game_logic__util_GRAVITY_SOURCES_BINDING, environment.state.gravity_sources_buffer);
		}

		{ // Count buffer
			GLuint const block_index
			{
				glGetProgramResourceIndex(environment.state.rigid_body_velocity_integration_shader, GL_UNIFORM_BLOCK, "Count")
			};
			GLenum const buffer_size_label{ GL_BUFFER_DATA_SIZE };
			glGetProgramResourceiv
			(
				environment.state.rigid_body_velocity_integration_shader, GL_UNIFORM_BLOCK, block_index,
				1u, &buffer_size_label, 1u, nullptr, &environment.state.count_buffer_size
			);

			GLenum const offset_label{ GL_OFFSET };

			GLuint const bodies_index
			{
				glGetProgramResourceIndex(environment.state.rigid_body_velocity_integration_shader, GL_UNIFORM, "Count.bodies")
			};
			glGetProgramResourceiv
			(
				environment.state.rigid_body_velocity_integration_shader, GL_UNIFORM, bodies_index,
				1u, &offset_label, 1u, nullptr, &environment.state.count_buffer_bodies_offset
			);

			GLuint const triangles_index
			{
				glGetProgramResourceIndex(environment.state.rigid_body_velocity_integration_shader, GL_UNIFORM, "Count.triangles")
			};
			glGetProgramResourceiv
			(
				environment.state.rigid_body_velocity_integration_shader, GL_UNIFORM, triangles_index,
				1u, &offset_label, 1u, nullptr, &environment.state.count_buffer_triangles_offset
			);

			GLuint const fluid_particles_index
			{
				glGetProgramResourceIndex(environment.state.rigid_body_velocity_integration_shader, GL_UNIFORM, "Count.fluid_particles")
			};
			glGetProgramResourceiv
			(
				environment.state.rigid_body_velocity_integration_shader, GL_UNIFORM, fluid_particles_index,
				1u, &offset_label, 1u, nullptr, &environment.state.count_buffer_fluid_particles_offset
			);

			unsigned char* initial_count = new unsigned char[environment.state.count_buffer_size];
			std::memcpy
			(
				initial_count + environment.state.count_buffer_bodies_offset, 
				&environment.state.current_rigid_body_count, 
				sizeof(GLuint)
			);
			std::memcpy
			(
				initial_count + environment.state.count_buffer_triangles_offset,
				&environment.state.GPU_buffers.rigid_bodies.triangles.current_count,
				sizeof(GLuint)
			);
			std::memcpy
			(
				initial_count + environment.state.count_buffer_fluid_particles_offset, 
				&environment.state.current_fluid_particle_count,
				sizeof(GLuint)
			);
			glNamedBufferStorage
			(
				environment.state.count_buffer, environment.state.count_buffer_size, initial_count,
				0u
			);
			delete[] initial_count;

			glBindBufferBase(GL_UNIFORM_BUFFER, game_logic__util_COUNT_BINDING, environment.state.count_buffer);
		}

		{ // Body masses buffer
			GLuint const masses_index
			{
				glGetProgramResourceIndex(environment.state.solve_contact_velocities_shader, GL_BUFFER_VARIABLE, "Body_Masses.masses")
			};

			GLenum const prop_labels[]{ GL_OFFSET, GL_ARRAY_STRIDE };
			GLint props[std::size(prop_labels)];
			glGetProgramResourceiv
			(
				environment.state.solve_contact_velocities_shader, GL_BUFFER_VARIABLE, masses_index,
				std::size(prop_labels), prop_labels, 2u, nullptr, props
			);
			// TODO: Consider putting offset and stride contigously in game state
			environment.state.body_masses_buffer_masses_offset = props[0u];
			environment.state.body_masses_buffer_masses_stride = props[1u];

#if USE_DYNAMIC_SIZES == true
			environment.state.body_masses_buffer_size = environment.state.body_masses_buffer_masses_offset + game_logic_MAX_RIGID_BODY_COUNT(environment) * environment.state.body_masses_buffer_masses_stride;
#else
			GLuint const block_index
			{
				glGetProgramResourceIndex(environment.state.solve_contact_velocities_shader, GL_SHADER_STORAGE_BLOCK, "Body_Masses")
			};
			GLenum const buffer_size_label{ GL_BUFFER_DATA_SIZE };
			glGetProgramResourceiv
			(
				environment.state.solve_contact_velocities_shader, GL_SHADER_STORAGE_BLOCK, block_index,
				1u, &buffer_size_label, 1u, nullptr, &environment.state.body_masses_buffer_size
			);
#endif

			glNamedBufferStorage
			(
				environment.state.body_masses_buffer, environment.state.body_masses_buffer_size, nullptr,
				0u
			);

			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, game_logic__util_BODY_MASSES_BINDING, environment.state.body_masses_buffer);
		}

		util::proximity::initialize
		(
			environment.state.proximity_tree, game_logic_MAX_LEAF_COUNT(environment), 
			MAX_CONTACT_COUNT(environment)
		);
		util::proximity::insert_leaf_to_empty_tree(
			environment.state.proximity_tree, game_logic_MAX_LEAF_COUNT(environment),
			0, 
			0, 0, -1, -1
		);
		for (GLuint i{ 1u }; i < environment.state.current_fluid_particle_count; ++i)
		{
			util::proximity::insert_leaf_to_nonempty_tree(
				environment.state.proximity_tree, game_logic_MAX_LEAF_COUNT(environment),
				i, 
				0, 0, -1, -1
			);
		}

		GLuint const current_triangle_leaf_index_end{ game_logic_TRIANGLE_LEAFS_BASE_INDEX(environment) + environment.state.GPU_buffers.rigid_bodies.triangles.current_count };
		for (GLuint i{ game_logic_TRIANGLE_LEAFS_BASE_INDEX(environment) }; i < current_triangle_leaf_index_end; ++i)
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

		std::cout << "Position buffer (" << environment.state.GPU_buffers.rigid_bodies.positions.buffer << "):" << std::endl;
		std::cout << "size: " << environment.state.GPU_buffers.rigid_bodies.positions.size << std::endl;
		std::cout << "p offset: " << environment.state.GPU_buffers.rigid_bodies.positions.p_offset << std::endl;
		std::cout << "p stride: " << environment.state.GPU_buffers.rigid_bodies.positions.p_stride << std::endl;
		std::cout << std::endl;

		std::cout << "Velocity buffer (" << environment.state.GPU_buffers.rigid_bodies.velocities.buffer << "):" << std::endl;
		std::cout << "size: " << environment.state.GPU_buffers.rigid_bodies.velocities.size << std::endl;
		std::cout << "v offset: " << environment.state.GPU_buffers.rigid_bodies.velocities.v_offset << std::endl;
		std::cout << "v stride: " << environment.state.GPU_buffers.rigid_bodies.velocities.v_stride << std::endl;
		std::cout << std::endl;

		std::cout << "Triangle buffer (" << environment.state.GPU_buffers.rigid_bodies.triangles.buffer << "):" << std::endl;
		std::cout << "size: " << environment.state.GPU_buffers.rigid_bodies.triangles.size << std::endl;
		std::cout << "triangles offset: " << environment.state.GPU_buffers.rigid_bodies.triangles.triangles_offset << std::endl;
		std::cout << "triangles stride: " << environment.state.GPU_buffers.rigid_bodies.triangles.triangles_stride << std::endl;
		std::cout << std::endl;

		std::cout << "Vertex buffer (" << environment.state.GPU_buffers.rigid_bodies.triangles.vertices.buffer << "):" << std::endl;
		std::cout << "size: " << environment.state.GPU_buffers.rigid_bodies.triangles.vertices.size << std::endl;
		std::cout << "vertices offset: " << environment.state.GPU_buffers.rigid_bodies.triangles.vertices.vertices_offset << std::endl;
		std::cout << "vertices stride: " << environment.state.GPU_buffers.rigid_bodies.triangles.vertices.vertices_stride << std::endl;
		std::cout << std::endl;

		std::cout << "Bounding box buffer (" << environment.state.GPU_buffers.rigid_bodies.triangles.bounding_boxes.buffer << "):" << std::endl;
		std::cout << "size: " << environment.state.GPU_buffers.rigid_bodies.triangles.bounding_boxes.size << std::endl;
		std::cout << "boxes offset: " << environment.state.GPU_buffers.rigid_bodies.triangles.bounding_boxes.boxes_offset << std::endl;
		std::cout << "boxes stride: " << environment.state.GPU_buffers.rigid_bodies.triangles.bounding_boxes.boxes_stride << std::endl;
		std::cout << std::endl;

		std::cout << "Changed bounding box buffer (" << environment.state.GPU_buffers.rigid_bodies.triangles.changed_bounding_boxes.buffer << "):" << std::endl;
		std::cout << "size: " << environment.state.GPU_buffers.rigid_bodies.triangles.changed_bounding_boxes.size << std::endl;
		std::cout << "push index offset: " << environment.state.GPU_buffers.rigid_bodies.triangles.changed_bounding_boxes.size_offset << std::endl;
		std::cout << "boxes stride: " << environment.state.GPU_buffers.rigid_bodies.triangles.changed_bounding_boxes.boxes_stride << std::endl;
		std::cout << "boxes index offset: " << environment.state.GPU_buffers.rigid_bodies.triangles.changed_bounding_boxes.boxes_index_offset << std::endl;
		std::cout << "boxes min_x offset: " << environment.state.GPU_buffers.rigid_bodies.triangles.changed_bounding_boxes.boxes_min_x_offset << std::endl;
		std::cout << "boxes min_y offset: " << environment.state.GPU_buffers.rigid_bodies.triangles.changed_bounding_boxes.boxes_min_y_offset << std::endl;
		std::cout << "boxes max_x offset: " << environment.state.GPU_buffers.rigid_bodies.triangles.changed_bounding_boxes.boxes_max_x_offset << std::endl;
		std::cout << "boxes max_y offset: " << environment.state.GPU_buffers.rigid_bodies.triangles.changed_bounding_boxes.boxes_max_y_offset << std::endl;
		std::cout << std::endl;

		std::cout << "Contact buffer (" << environment.state.contact_buffer << "):" << std::endl;
		std::cout << "size: " << environment.state.contact_buffer_size << std::endl;
		std::cout << "contacts offset: " << environment.state.contact_buffer_contacts_offset << std::endl;
		std::cout << "contacts stride: " << environment.state.contact_buffer_contacts_stride << std::endl;
		std::cout << std::endl;

		std::cout << "Contact surfaces buffer (" << environment.state.contact_surface_buffer << "):" << std::endl;
		std::cout << "size: " << environment.state.GPU_buffers.rigid_bodies.triangles.contact_surfaces.size << std::endl;
		std::cout << "contact surfaces stride: " << environment.state.contact_surface_buffer_contact_surfaces_stride << std::endl;
		std::cout << "contact surfaces bodies offset: " << environment.state.GPU_buffers.rigid_bodies.triangles.contact_surfaces.contact_surfaces_bodies_offset << std::endl;
		std::cout << "contact surfaces bodies stride: " << environment.state.GPU_buffers.rigid_bodies.triangles.contact_surfaces.contact_surfaces_bodies_stride << std::endl;
		std::cout << "contact surfaces contact point position 0 offsets offset: " << environment.state.GPU_buffers.rigid_bodies.triangles.contact_surfaces.contact_surfaces_contact_point_position_0_offsets_offset << std::endl;
		std::cout << "contact surfaces contact point position 0 offsets stride: " << environment.state.GPU_buffers.rigid_bodies.triangles.contact_surfaces.contact_surfaces_contact_point_position_0_offsets_stride << std::endl;
		std::cout << "contact surfaces contact point position 1 offsets offset: " << environment.state.GPU_buffers.rigid_bodies.triangles.contact_surfaces.contact_surfaces_contact_point_position_1_offsets_offset << std::endl;
		std::cout << "contact surfaces contact point position 1 offsets stride: " << environment.state.GPU_buffers.rigid_bodies.triangles.contact_surfaces.contact_surfaces_contact_point_position_1_offsets_stride << std::endl;
		std::cout << "contact surfaces tangent offset: " << environment.state.GPU_buffers.rigid_bodies.triangles.contact_surfaces.contact_surfaces_tangent_offset << std::endl;
		std::cout << "contact surfaces contact point tangent 0 mass offset: " << environment.state.GPU_buffers.rigid_bodies.triangles.contact_surfaces.contact_surfaces_contact_point_tangent_0_mass_offset << std::endl;
		std::cout << "contact surfaces contact point tangent 0 impulse offset: " << environment.state.GPU_buffers.rigid_bodies.triangles.contact_surfaces.contact_surfaces_contact_point_tangent_0_impulse_offset << std::endl;
		std::cout << "contact surfaces contact point tangent 1 mass offset: " << environment.state.GPU_buffers.rigid_bodies.triangles.contact_surfaces.contact_surfaces_contact_point_tangent_1_mass_offset << std::endl;
		std::cout << "contact surfaces contact point tangent 1 impulse offset: " << environment.state.GPU_buffers.rigid_bodies.triangles.contact_surfaces.contact_surfaces_contact_point_tangent_1_impulse_offset << std::endl;
		std::cout << "contact surfaces contact point normal 0 target velocity offset: " << environment.state.GPU_buffers.rigid_bodies.triangles.contact_surfaces.contact_surfaces_contact_point_normal_0_target_velocity_offset << std::endl;
		std::cout << "contact surfaces contact point normal 0 mass offset: " << environment.state.GPU_buffers.rigid_bodies.triangles.contact_surfaces.contact_surfaces_contact_point_normal_0_mass_offset << std::endl;
		std::cout << "contact surfaces contact point normal 0 impulse offset: " << environment.state.GPU_buffers.rigid_bodies.triangles.contact_surfaces.contact_surfaces_contact_point_normal_0_impulse_offset << std::endl;
		std::cout << "contact surfaces contact point normal 1 target velocity offset: " << environment.state.GPU_buffers.rigid_bodies.triangles.contact_surfaces.contact_surfaces_contact_point_normal_1_target_velocity_offset << std::endl;
		std::cout << "contact surfaces contact point normal 1 mass offset: " << environment.state.GPU_buffers.rigid_bodies.triangles.contact_surfaces.contact_surfaces_contact_point_normal_1_mass_offset << std::endl;
		std::cout << "contact surfaces contact point normal 1 impulse offset: " << environment.state.GPU_buffers.rigid_bodies.triangles.contact_surfaces.contact_surfaces_contact_point_normal_1_impulse_offset << std::endl;
		std::cout << "contact surfaces offset: " << environment.state.contact_surface_buffer_contact_surfaces_offset << std::endl;
		std::cout << std::endl;

		std::cout << "Contact count buffer (" << environment.state.GPU_buffers.rigid_bodies.triangles.contact_count.buffer << "):" << std::endl;
		std::cout << "size: " << environment.state.GPU_buffers.rigid_bodies.triangles.contact_count.size << std::endl;
		std::cout << "contact count offset: " << environment.state.GPU_buffers.rigid_bodies.triangles.contact_count.contact_count_offset << std::endl;
		std::cout << std::endl;

		std::cout << "Persistent contact count buffer (" << environment.state.GPU_buffers.rigid_bodies.triangles.persistent_contact_count.buffer << "):" << std::endl;
		std::cout << "size: " << environment.state.GPU_buffers.rigid_bodies.triangles.persistent_contact_count.size << std::endl;
		std::cout << "persistent contact count offset: " << environment.state.GPU_buffers.rigid_bodies.triangles.persistent_contact_count.persistent_contact_count_offset << std::endl;
		std::cout << std::endl;

		std::cout << "Cursor position buffer (" << environment.state.cursor_position_buffer << "):" << std::endl;
		std::cout << "size: " << environment.state.cursor_position_buffer_size << std::endl;
		std::cout << "position offset: " << environment.state.cursor_position_buffer_position_offset << std::endl;
		std::cout << std::endl;

		std::cout << "Cursor constrained point buffer (" << environment.state.cursor_constrained_point_buffer << "):" << std::endl;
		std::cout << "size: " << environment.state.cursor_constrained_point_buffer_size << std::endl;
		std::cout << "body offset: " << environment.state.cursor_constrained_point_buffer_body_offset << std::endl;
		std::cout << "local point offset: " << environment.state.cursor_constrained_point_buffer_local_point_offset << std::endl;
		std::cout << std::endl;

		std::cout << "Cursor constraint buffer (" << environment.state.cursor_constraint_buffer << "):" << std::endl;
		std::cout << "size: " << environment.state.cursor_constraint_buffer_size << std::endl;
		std::cout << "offset offset: " << environment.state.cursor_constraint_buffer_offset_offset << std::endl;
		std::cout << "target velocity offset: " << environment.state.cursor_constraint_buffer_target_velocity_offset << std::endl;
		std::cout << "mass offset: " << environment.state.cursor_constraint_buffer_mass_offset << std::endl;
		std::cout << "mass matrix stride: " << environment.state.cursor_constraint_buffer_mass_matrix_stride << std::endl;
		std::cout << "impulse offset: " << environment.state.cursor_constraint_buffer_impulse_offset << std::endl;
		std::cout << std::endl;

		std::cout << "Distance constraint buffer (" << environment.state.GPU_buffers.rigid_bodies.distance_constraints.buffer << "):" << std::endl;
		std::cout << "size: " << environment.state.GPU_buffers.rigid_bodies.distance_constraints.size << std::endl;
		std::cout << "count offset: " << environment.state.GPU_buffers.rigid_bodies.distance_constraints.count_offset << std::endl;
		std::cout << "distance constraints offset: " << environment.state.GPU_buffers.rigid_bodies.distance_constraints.distance_constraints_offset << std::endl;
		std::cout << "distance constraints stride: " << environment.state.GPU_buffers.rigid_bodies.distance_constraints.distance_constraints_stride << std::endl;
		std::cout << "distance constraints bodies offset: " << environment.state.GPU_buffers.rigid_bodies.distance_constraints.distance_constraints_bodies_offset << std::endl;
		std::cout << "distance constraints local points offset: " << environment.state.GPU_buffers.rigid_bodies.distance_constraints.distance_constraints_local_points_offset << std::endl;
		std::cout << "distance constraints local points stride: " << environment.state.GPU_buffers.rigid_bodies.distance_constraints.distance_constraints_local_points_stride << std::endl;
		std::cout << "distance constraints offsets offset: " << environment.state.GPU_buffers.rigid_bodies.distance_constraints.distance_constraints_offsets_offset << std::endl;
		std::cout << "distance constraints offsets stride: " << environment.state.GPU_buffers.rigid_bodies.distance_constraints.distance_constraints_offsets_stride << std::endl;
		std::cout << "distance constraints direction offset: " << environment.state.GPU_buffers.rigid_bodies.distance_constraints.distance_constraints_direction_offset << std::endl;
		std::cout << "distance constraints max distance offset: " << environment.state.GPU_buffers.rigid_bodies.distance_constraints.distance_constraints_max_distance_offset << std::endl;
		std::cout << "distance constraints target velocity offset: " << environment.state.GPU_buffers.rigid_bodies.distance_constraints.distance_constraints_target_velocity_offset << std::endl;
		std::cout << "distance constraints mass offset: " << environment.state.GPU_buffers.rigid_bodies.distance_constraints.distance_constraints_mass_offset << std::endl;
		std::cout << "distance constraints impulse offset: " << environment.state.GPU_buffers.rigid_bodies.distance_constraints.distance_constraints_impulse_offset << std::endl;
		std::cout << std::endl;

		std::cout << "Fluid position buffer (" << environment.state.GPU_buffers.fluid.positions.buffer << "):" << std::endl;
		std::cout << "size: " << environment.state.GPU_buffers.fluid.positions.size << std::endl;
		std::cout << "p offset: " << environment.state.GPU_buffers.fluid.positions.p_offset << std::endl;
		std::cout << "p stride: " << environment.state.GPU_buffers.fluid.positions.p_stride << std::endl;
		std::cout << std::endl;

		std::cout << "Fluid velocity buffer (" << environment.state.GPU_buffers.fluid.velocities.buffer << "):" << std::endl;
		std::cout << "size: " << environment.state.GPU_buffers.fluid.velocities.size << std::endl;
		std::cout << "v offset: " << environment.state.GPU_buffers.fluid.velocities.v_offset << std::endl;
		std::cout << "v stride: " << environment.state.GPU_buffers.fluid.velocities.v_stride << std::endl;
		std::cout << std::endl;

		std::cout << "Fluid bounding box buffer (" << environment.state.GPU_buffers.fluid.bounding_boxes.buffer << "):" << std::endl;
		std::cout << "size: " << environment.state.GPU_buffers.fluid.bounding_boxes.size << std::endl;
		std::cout << "boxes offset: " << environment.state.GPU_buffers.fluid.bounding_boxes.boxes_offset << std::endl;
		std::cout << "boxes stride: " << environment.state.GPU_buffers.fluid.bounding_boxes.boxes_stride << std::endl;
		std::cout << std::endl;

		std::cout << "Changed fluid bounding box buffer (" << environment.state.GPU_buffers.fluid.changed_bounding_boxes.buffer << "):" << std::endl;
		std::cout << "size: " << environment.state.GPU_buffers.fluid.changed_bounding_boxes.size << std::endl;
		std::cout << "push index offset: " << environment.state.GPU_buffers.fluid.changed_bounding_boxes.size_offset << std::endl;
		std::cout << "boxes stride: " << environment.state.GPU_buffers.fluid.changed_bounding_boxes.boxes_stride << std::endl;
		std::cout << "boxes index offset: " << environment.state.GPU_buffers.fluid.changed_bounding_boxes.boxes_index_offset << std::endl;
		std::cout << "boxes min_x offset: " << environment.state.GPU_buffers.fluid.changed_bounding_boxes.boxes_min_x_offset << std::endl;
		std::cout << "boxes min_y offset: " << environment.state.GPU_buffers.fluid.changed_bounding_boxes.boxes_min_y_offset << std::endl;
		std::cout << "boxes max_x offset: " << environment.state.GPU_buffers.fluid.changed_bounding_boxes.boxes_max_x_offset << std::endl;
		std::cout << "boxes max_y offset: " << environment.state.GPU_buffers.fluid.changed_bounding_boxes.boxes_max_y_offset << std::endl;
		std::cout << std::endl;

		std::cout << "Fluid contact buffer (" << environment.state.GPU_buffers.fluid.contacts.buffer << "):" << std::endl;
		std::cout << "size: " << environment.state.GPU_buffers.fluid.contacts.size << std::endl;
		std::cout << "contacts stride: " << environment.state.GPU_buffers.fluid.contacts.contacts_stride << std::endl;
		std::cout << "contacts intactive offset: " << environment.state.GPU_buffers.fluid.contacts.contacts_inactive_offset << std::endl;
		std::cout << "contacts impulse offset: " << environment.state.GPU_buffers.fluid.contacts.contacts_impulse_offset << std::endl;
		std::cout << "contacts particles offset: " << environment.state.GPU_buffers.fluid.contacts.contacts_particles_offset << std::endl;
		std::cout << "contacts direction offset: " << environment.state.GPU_buffers.fluid.contacts.contacts_direction_offset << std::endl;
		std::cout << "contacts target velocity offset: " << environment.state.GPU_buffers.fluid.contacts.contacts_target_velocity_offset << std::endl;
		std::cout << "contacts mass offset: " << environment.state.GPU_buffers.fluid.contacts.contacts_mass_offset << std::endl;
		std::cout << "contacts impulse range offset: " << environment.state.GPU_buffers.fluid.contacts.contacts_impulse_range_offset << std::endl;
		std::cout << std::endl;

		std::cout << "Fluid contact count buffer (" << environment.state.GPU_buffers.fluid.contact_count.buffer << "):" << std::endl;
		std::cout << "size: " << environment.state.GPU_buffers.fluid.contact_count.size << std::endl;
		//std::cout << "persistent count offset: " << environment.state.fluid_contact_count_buffer_persistent_count_offset << std::endl;
		std::cout << "count offset: " << environment.state.GPU_buffers.fluid.contact_count.count_offset << std::endl;
		std::cout << std::endl;

		std::cout << "Fluid triangle contact buffer (" << environment.state.GPU_buffers.fluid_triangle.contacts.buffer << "):" << std::endl;
		std::cout << "size: " << environment.state.GPU_buffers.fluid_triangle.contacts.size << std::endl;
		std::cout << "contacts offset: " << environment.state.GPU_buffers.fluid_triangle.contacts.contacts_offset << std::endl;
		std::cout << "contacts stride: " << environment.state.GPU_buffers.fluid_triangle.contacts.contacts_stride << std::endl;
		std::cout << "contacts triangle offset: " << environment.state.GPU_buffers.fluid_triangle.contacts.contacts_triangle_offset << std::endl;
		std::cout << "contacts mass offset: " << environment.state.GPU_buffers.fluid_triangle.contacts.contacts_mass_offset << std::endl;
		std::cout << "contacts impulse offset: " << environment.state.GPU_buffers.fluid_triangle.contacts.contacts_impulse_offset << std::endl;
		std::cout << "contacts particle offset: " << environment.state.GPU_buffers.fluid_triangle.contacts.contacts_particle_offset << std::endl;
		std::cout << "contacts body offset: " << environment.state.GPU_buffers.fluid_triangle.contacts.contacts_body_offset << std::endl;
		std::cout << "contacts target velocity offset: " << environment.state.GPU_buffers.fluid_triangle.contacts.contacts_target_velocity_offset << std::endl;
		std::cout << "contacts offset offset: " << environment.state.GPU_buffers.fluid_triangle.contacts.contacts_offset_offset << std::endl;
		std::cout << "contacts direction offset: " << environment.state.GPU_buffers.fluid_triangle.contacts.contacts_direction_offset << std::endl;
		std::cout << "contacts impulse range offset: " << environment.state.GPU_buffers.fluid_triangle.contacts.contacts_impulse_range_offset << std::endl;
		std::cout << std::endl;

		std::cout << "Fluid triangle contact count buffer (" << environment.state.GPU_buffers.fluid_triangle.contact_count.buffer << "):" << std::endl;
		std::cout << "size: " << environment.state.GPU_buffers.fluid_triangle.contact_count.size << std::endl;
		std::cout << "count offset: " << environment.state.GPU_buffers.fluid_triangle.contact_count.count_offset << std::endl;
		std::cout << std::endl;

		std::cout << "Gravity sources buffer (" << environment.state.gravity_sources_buffer << "):" << std::endl;
		std::cout << "size: " << environment.state.gravity_sources_buffer_size << std::endl;
		std::cout << "count offset: " << environment.state.gravity_sources_buffer_count_offset << std::endl;
		std::cout << "positions offset: " << environment.state.gravity_sources_buffer_positions_offset << std::endl;
		std::cout << "positions stride: " << environment.state.gravity_sources_buffer_positions_stride << std::endl;
		std::cout << "strengths offset: " << environment.state.gravity_sources_buffer_strengths_offset << std::endl;
		std::cout << "strengths stride: " << environment.state.gravity_sources_buffer_strengths_stride << std::endl;
		std::cout << std::endl;

		std::cout << "Count buffer (" << environment.state.count_buffer << "):" << std::endl;
		std::cout << "size: " << environment.state.count_buffer_size << std::endl;
		std::cout << "bodies offset: " << environment.state.count_buffer_bodies_offset << std::endl;
		std::cout << "triangles offset: " << environment.state.count_buffer_triangles_offset << std::endl;
		std::cout << "fluid particles offset: " << environment.state.count_buffer_fluid_particles_offset << std::endl;
		std::cout << std::endl;

		std::cout << "Body masses buffer (" << environment.state.body_masses_buffer << "):" << std::endl;
		std::cout << "size: " << environment.state.body_masses_buffer_size << std::endl;
		std::cout << "masses offset: " << environment.state.body_masses_buffer_masses_offset << std::endl;
		std::cout << "masses stride: " << environment.state.body_masses_buffer_masses_stride << std::endl;
		std::cout << std::endl;

		Model<3u> triangle_model;
		GLfloat triangle_vertices[][2u]
		{
			{ 0.5f, 0.5f },
			{ -0.5f, 0.5f },
			{ -0.5f, -0.5f },
		};
		GLuint triangle_vertex_indices[]
		{
			0u, 1u, 2u,
		};
		create_model<3u, 3u>(environment, 4u, triangle_vertices, triangle_vertex_indices, triangle_model);

		Model<6u> box_model;
		GLfloat box_vertices[][2u]
		{
			{ 0.5f, 0.5f },
			{ -0.5f, 0.5f },
			{ -0.5f, -0.5f },
			{ 0.5f, -0.5f },
		};
		GLuint box_vertex_indices[]
		{
			0u, 1u, 2u,
			2u, 3u, 0u
		};
		create_model<4u, 6u>(environment, 7u, box_vertices, box_vertex_indices, box_model);

		Model<9u> house_model;
		GLfloat house_vertices[][2u]
		{
			{ 0.5f, 0.5f },
			{ -0.5f, 0.5f },
			{ -0.5f, -1.0f },
			{ 0.5f, -1.0f },
			{ 0.0f, 1.5f },
			{ -1.0f, 0.5f },
			{ 1.0f, 0.5f },
		};
		GLuint house_vertex_indices[]
		{
			0u, 1u, 2u,
			2u, 3u, 0u, 
			4u, 5u, 6u
		};
		create_model<7u, 9u>(environment, 11u, house_vertices, house_vertex_indices, house_model);
		
		GLuint const planet_outline_vertex_count{ 16u };
		Model<planet_outline_vertex_count * 3u> planet_model;
		{
			GLfloat planet_vertices[planet_outline_vertex_count + 1u][2u];
			planet_vertices[0u][0u] = 0.0f;
			planet_vertices[0u][1u] = 0.0f;
			GLfloat planet_outline_distances[planet_outline_vertex_count]
			{
				2.0f, 3.0f, 3.5f, 2.5f, 1.5f, 2.0f, 2.5f, 3.3f,
				5.0f, 4.0f, 2.7f, 3.2f, 2.9f, 2.6f, 2.3f, 1.8f
			};
			GLfloat const angle_step{ 2.0f * M_PI / planet_outline_vertex_count };
			for (GLuint i{ 0u }; i < planet_outline_vertex_count; ++i)
			{
				GLfloat const angle{ static_cast<GLfloat>(i) * angle_step };
				GLfloat const distance{ planet_outline_distances[i] };
				GLfloat const x{ std::cos(angle) * distance };
				GLfloat const y{ std::sin(angle) * distance };
				planet_vertices[i + 1u][0u] = x;
				planet_vertices[i + 1u][1u] = y;
			}
			GLuint planet_vertex_indices[planet_outline_vertex_count * 3u];
			for (GLuint i{ 0u }; i < planet_outline_vertex_count; ++i)
			{
				GLuint j{ i * 3u };
				planet_vertex_indices[j] = 0u;
				planet_vertex_indices[j + 1u] = i + 1u;
				planet_vertex_indices[j + 2u] = ((i + 1u) % planet_outline_vertex_count) + 1u;
			}
			create_model<planet_outline_vertex_count + 1u, planet_outline_vertex_count * 3u>
			(
				environment, 18u, planet_vertices, planet_vertex_indices, planet_model
			);
		}

		GLuint const big_planet_outline_vertex_count{ 16u };
		Model<big_planet_outline_vertex_count * 3u> big_planet_model;
		{
			GLfloat big_planet_vertices[big_planet_outline_vertex_count + 1u][2u];
			big_planet_vertices[0u][0u] = 0.0f;
			big_planet_vertices[0u][1u] = 0.0f;
			GLfloat big_planet_outline_distances[planet_outline_vertex_count]
			{
				9.0f, 8.5f, 9.5f, 8.5f, 6.5f, 7.0f, 6.5f, 7.3f,
				8.0f, 6.0f, 6.7f, 8.2f, 9.9f, 6.6f, 5.3f, 9.8f
			};
			GLfloat const angle_step{ 2.0f * M_PI / big_planet_outline_vertex_count };
			for (GLuint i{ 0u }; i < big_planet_outline_vertex_count; ++i)
			{
				GLfloat const angle{ static_cast<GLfloat>(i) * angle_step };
				GLfloat const distance{ big_planet_outline_distances[i] };
				GLfloat const x{ std::cos(angle) * distance };
				GLfloat const y{ std::sin(angle) * distance };
				big_planet_vertices[i + 1u][0u] = x;
				big_planet_vertices[i + 1u][1u] = y;
			}
			GLuint big_planet_vertex_indices[planet_outline_vertex_count * 3u];
			for (GLuint i{ 0u }; i < big_planet_outline_vertex_count; ++i)
			{
				GLuint j{ i * 3u };
				big_planet_vertex_indices[j] = 0u;
				big_planet_vertex_indices[j + 1u] = i + 1u;
				big_planet_vertex_indices[j + 2u] = ((i + 1u) % big_planet_outline_vertex_count) + 1u;
			}
			create_model<big_planet_outline_vertex_count + 1u, big_planet_outline_vertex_count * 3u>
			(
				environment, 19u + planet_outline_vertex_count, big_planet_vertices, big_planet_vertex_indices, big_planet_model
			);
		}

		Model<18u> bucket_model;
		GLfloat const w{ 2.0f };
		GLfloat const t{ 0.5f };
		GLfloat const h{ t + 4.0f };
		GLfloat bucket_vertices[][2u]
		{
			{ w, t },
			{ -w, t },
			{ -w, -t },
			{ w, -t },

			{ -w, h },
			{ -w - 2.0f * t, h },
			{ -w - 2.0f * t, -t },

			{ w + 2.0f * t, -t },
			{ w + 2.0f * t, h },
			{ w, h },
		};
		GLuint bucket_vertex_indices[]
		{
			0u, 1u, 2u,
			2u, 3u, 0u, 

			4u, 5u, 6u, 
			6u, 2u, 4u, 

			3u, 7u, 8u, 
			8u, 9u, 3u
		};
		create_model<10u, 18u>(environment, 20u + planet_outline_vertex_count + big_planet_outline_vertex_count, bucket_vertices, bucket_vertex_indices, bucket_model);

		Model<3u> long_triangle_model;
		GLfloat long_triangle_vertices[][2u]
		{
			{ 1.0f, 0.5f },
			{ -0.5f, 0.5f },
			{ -1.0f, -0.7f },
		};
		GLuint long_triangle_vertex_indices[]
		{
			0u, 1u, 2u,
		};
		create_model<3u, 3u>(environment, 20u + planet_outline_vertex_count + big_planet_outline_vertex_count + 10, long_triangle_vertices, long_triangle_vertex_indices, long_triangle_model);

		instantiate_model
		(
			environment, planet_model,
			-10 * 1000000, 10 * 1000000, 0,
			0, 0, 0
		);

		instantiate_model
		(
			environment, planet_model,
			-20 * 1000000, 0, 1 * 1000000,
			0, 0, 0
		);

		instantiate_model
		(
			environment, big_planet_model,
			-50 * 1000000, 50 * 1000000, 1 * 1000000,
			0, 0, 0
		);

		instantiate_model
		(
			environment, bucket_model, 
			20 * 1000000, -5000000, 0, 
			0, 0, 0
		);

		//instantiate_model(environment, box_model, 0, 0, 0, 0, 0, 0);
		//instantiate_model(environment, triangle_model, 10000000, 0, 0, 0, 0, 0);

		for (GLuint i{ 0u }; i < 60u * game_logic__util__rigid_body_DEFAULT_COMPUTE_SHADER_LOCAL_SIZE(environment); ++i)
		{
			GLuint const width{ 100u };
			
			GLint x{ game_logic__util__spatial_FROM_METERS(environment, ((i % width) * 2.0f))};
			GLint y{ game_logic__util__spatial_FROM_METERS(environment, ((i / width) * 2.0f)) };
			GLint angle{ game_logic__util__spatial_FROM_RADIANS(environment, i * 0.1f) };
			
			GLint vx{ 0 };
			GLint vy{ 0 };
			GLint va{ 0 };
			
			if (i % 8u == 0u)
			{
				instantiate_model
				(
					environment, box_model,
					x, y, angle,
					vx, vy, va
				);
			}
			else if (i % 37u == 0)
			{
				instantiate_model
				(
					environment, house_model, 
					x, y, angle, 
					vx, vy, va
				);
			}
			else if (i % 29u == 0)
			{
				instantiate_model
				(
					environment, long_triangle_model,
					x, y, angle,
					vx, vy, va
				);
			}
			else
			{
				instantiate_model
				(
					environment, triangle_model,
					x, y, angle,
					vx, vy, va
				);
			}
		}

		std::cout << "Rigid body count: " << environment.state.current_rigid_body_count << std::endl;
		std::cout << "Triangle count: " << environment.state.GPU_buffers.rigid_bodies.triangles.current_count << std::endl;
		std::cout << "Fluid particle count: " << environment.state.current_fluid_particle_count << std::endl;

		/*GLuint i;
		for (i = 0u; i < 30u * game_logic__util__rigid_body_DEFAULT_COMPUTE_SHADER_LOCAL_SIZE(environment); ++i)
		{
			GLuint const width{ 100u };
			instantiate_model
			(
				environment, triangle_model, 
				game_logic__util__spatial_FROM_METERS(environment, (i % width) * 2.0f), 
				game_logic__util__spatial_FROM_METERS(environment, (i / width) * 2.0f),
				game_logic__util__spatial_FROM_RADIANS(environment, i * 0.1f), 
				0, 0, 0
			);
		}

		for (; i < 40u * game_logic__util__rigid_body_DEFAULT_COMPUTE_SHADER_LOCAL_SIZE(environment); ++i)
		{
			GLuint const width{ 100u };
			instantiate_model
			(
				environment, house_model,
				game_logic__util__spatial_FROM_METERS(environment, (i % width) * 2.0f),
				game_logic__util__spatial_FROM_METERS(environment, (i / width) * 2.0f),
				game_logic__util__spatial_FROM_RADIANS(environment, i * 0.1f),
				0, 0, 0
			);
		}*/

		glMemoryBarrier(GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT);
		environment.state.physics_tick_results_fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0u);
		glFlush();
	}

	// TODO: Move to ::util::math
	inline GLuint ceil_div(GLuint numerator, GLuint denominator)
	{
		return numerator / denominator + (numerator % denominator != 0);
	}

	void integrate_forces(game_environment::Environment& environment)
	{
		// TODO: This memory barrier is only needed if it hasn't already been issued 
		// by a draw call right before this tick.
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);	// Last tick data for both bodies and constraints
	}

	void warm_start_constraint_impulses(game_environment::Environment& environment)
	{
		glUseProgram(environment.state.warm_start_fluid_contacts_shader);
		glDispatchCompute
		(
			ceil_div(environment.state.GPU_buffers.fluid.contact_count.current_contact_count, WARM_START_FLUID_CONTACTS_LOCAL_SIZE(environment)),
			1u, 1u
		);

		glUseProgram(environment.state.warm_start_fluid_triangle_contacts_shader);
		glDispatchCompute
		(
			ceil_div(environment.state.GPU_buffers.fluid_triangle.contact_count.current_contact_count, WARM_START_FLUID_TRIANGLE_CONTACTS_LOCAL_SIZE(environment)),
			1u, 1u
		);

		glUseProgram(environment.state.warm_start_contact_impulses_shader);
		glDispatchCompute
		(
			ceil_div(environment.state.current_persistent_contact_count, game_logic__util__rigid_body_WARM_START_CONTACT_IMPULSES_LOCAL_SIZE(environment)), 
			1u, 1u
		);
		
		glUseProgram(environment.state.warm_start_distance_constraints_shader);
		glDispatchCompute
		(
			ceil_div(environment.state.current_distance_constraint_count, game_logic_WARM_START_DISTANCE_CONSTRAINTS_LOCAL_SIZE(environment)), 
			1u, 1u
		);

		if (environment.state.grabbed_triangle != game_logic__util__proximity_NULL_INDEX)
		{
			glUseProgram(environment.state.update_and_warm_start_cursor_constraint_shader);
			glDispatchCompute(1u, 1u, 1u);
		}
	}

	void improve_constraint_impulses(game_environment::Environment& environment)
	{
		GLuint const solve_fluid_contacts_work_group_count
		{
			ceil_div(environment.state.GPU_buffers.fluid.contact_count.current_contact_count, SOLVE_FLUID_CONTACTS_LOCAL_SIZE(environment))
		};
		GLuint const solve_fluid_triangle_contacts_work_group_count
		{
			ceil_div(environment.state.GPU_buffers.fluid_triangle.contact_count.current_contact_count, SOLVE_FLUID_TRIANGLE_CONTACTS_LOCAL_SIZE(environment))
		};
		GLuint const solve_contact_velocities_work_group_count
		{
			ceil_div(environment.state.current_triangle_contact_count, game_logic__util__rigid_body_SOLVE_CONTACT_VELOCITIES_LOCAL_SIZE(environment))
		};
		GLuint const solve_distance_constraint_work_group_count
		{
			ceil_div(environment.state.current_distance_constraint_count, game_logic_SOLVE_DISTANCE_CONSTRAINTS_LOCAL_SIZE(environment))
		};
		for (GLuint i{ 0u }; i < 4u; ++i)
		{
			glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT); // Updated velocities

			glCopyNamedBufferSubData
			(
				environment.state.GPU_buffers.fluid.velocities.buffer, environment.state.GPU_buffers.fluid.velocities.snapshot_buffer,
				environment.state.GPU_buffers.fluid.velocities.v_offset, environment.state.GPU_buffers.fluid.velocities.v_offset,
				environment.state.current_fluid_particle_count * environment.state.GPU_buffers.fluid.velocities.v_stride
			);

			glUseProgram(environment.state.solve_fluid_contacts_shader);
			glDispatchCompute(solve_fluid_contacts_work_group_count, 1u, 1u);

			glCopyNamedBufferSubData
			(
				environment.state.GPU_buffers.rigid_bodies.velocities.buffer, environment.state.GPU_buffers.rigid_bodies.velocities.snapshot_buffer,
				environment.state.GPU_buffers.rigid_bodies.velocities.v_offset, environment.state.GPU_buffers.rigid_bodies.velocities.v_offset,
				environment.state.current_rigid_body_count * environment.state.GPU_buffers.rigid_bodies.velocities.v_stride
			);

			glUseProgram(environment.state.solve_fluid_triangle_contacts_shader);
			glDispatchCompute(solve_fluid_triangle_contacts_work_group_count, 1u, 1u);

			glUseProgram(environment.state.solve_contact_velocities_shader);
			glDispatchCompute(solve_contact_velocities_work_group_count, 1u, 1u);

			glUseProgram(environment.state.solve_distance_constraints_shader);
			glDispatchCompute(solve_distance_constraint_work_group_count, 1u, 1u);

			if (environment.state.grabbed_triangle != game_logic__util__proximity_NULL_INDEX)
			{
				glUseProgram(environment.state.solve_cursor_constraint_shader);
				glDispatchCompute(1u, 1u, 1u);
			}
		}
	}

	void tick_velocities(game_environment::Environment& environment)
	{
		integrate_forces(environment);
		warm_start_constraint_impulses(environment);
		improve_constraint_impulses(environment);
	}

	void tick_positions(game_environment::Environment& environment)
	{
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);	// Updated velocities

		glUseProgram(environment.state.integrate_fluid_velocity_shader);
		glDispatchCompute
		(
			ceil_div(environment.state.current_fluid_particle_count, INTEGRATE_FLUID_VELOCITY_LOCAL_SIZE(environment)),
			1u, 1u
		);
		// TODO: Make sure GL_BUFFER_UPDATE_BARRIER is the correct flag for clearing the changed leaf count
		glMemoryBarrier(GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT);	// Changed fluid bounding boxes
		GLsync const fluid_bounding_box_fence{ glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0u) };
		glFlush();
		// IMPORTANT TODO: I think the CPU is stalled on flush until all buffered commands have been submitted by the driver

		glUseProgram(environment.state.rigid_body_velocity_integration_shader);
		glDispatchCompute
		(
			ceil_div(environment.state.current_rigid_body_count, game_logic__util__rigid_body_VELOCITY_INTEGRATION_LOCAL_SIZE(environment)),
			1u, 1u
		);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);	// Positions and velocities from velocity integration

		glUseProgram(environment.state.triangle_bounding_box_update_shader);
		glDispatchCompute
		(
			ceil_div(environment.state.GPU_buffers.rigid_bodies.triangles.current_count, game_logic__util__rigid_body_TRIANGLE_BOUNDING_BOX_UPDATE_LOCAL_SIZE(environment)),
			1u, 1u
		);
		// TODO: Make sure GL_BUFFER_UPDATE_BARRIER is the correct flag for clearing the changed leaf count
		glMemoryBarrier(GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT);	// Changed bounding boxes
		GLsync const triangle_bounding_box_fence{ glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0u) };
		glFlush();

		/* If direct position correction is used:
		glCopyNamedBufferSubData
		(
			environment.state.rigid_body_position_buffer, environment.state.rigid_body_position_snapshot_buffer,
			environment.state.rigid_body_position_buffer_p_offset, environment.state.rigid_body_position_buffer_p_offset,
			environment.state.current_rigid_body_count * environment.state.rigid_body_position_buffer_p_stride
		);*/

		// TODO: Might not be needed
		// TODO: Consider making triangle contact update shader write to the snapshot 
		// so that this memory barrier becomes unnecessary.
		//glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);	// Writes to bounding boxes must occur before constraining the positions and velocities

		glUseProgram(environment.state.persist_fluid_contacts_shader);
		glDispatchCompute
		(
			ceil_div(environment.state.GPU_buffers.fluid.contact_count.current_contact_count, PERSIST_FLUID_CONTACT_LOCAL_SIZE(environment)),
			1u, 1u
		);

		glUseProgram(environment.state.persist_fluid_triangle_contacts_shader);
		glDispatchCompute
		(
			ceil_div(environment.state.GPU_buffers.fluid_triangle.contact_count.current_contact_count, PERSIST_FLUID_TRIANGLE_CONTACT_LOCAL_SIZE(environment)), 
			1u, 1u
		);

		// TODO: Optimize this shader! It's expensive! Also make sure we do not use any flags for the buffers
		glUseProgram(environment.state.old_triangle_contact_update_shader);
		glDispatchCompute
		(
			ceil_div(environment.state.current_triangle_contact_count, game_logic__util__rigid_body_OLD_TRIANGLE_CONTACT_UPDATE_LOCAL_SIZE(environment)),
			1u, 1u
		);

		glUseProgram(environment.state.update_distance_constraints_shader);
		glDispatchCompute
		(
			ceil_div(environment.state.current_distance_constraint_count, game_logic_UPDATE_DISTANCE_CONSTRAINTS_LOCAL_SIZE(environment)),
			1u, 1u
		);

		// TODO: Potential one-time CPU operations we must do before 
		// waiting for the GPU to be done.

		//auto t_0 = std::chrono::high_resolution_clock::now();
		GLenum fluid_bounding_box_fence_status = glClientWaitSync(fluid_bounding_box_fence, 0u, 0u);
		while (fluid_bounding_box_fence_status != GL_ALREADY_SIGNALED && fluid_bounding_box_fence_status != GL_CONDITION_SATISFIED)
		{
			// TODO: Do something useful but not necessary while we wait. 
			// Example: Optimize proximity tree.
			fluid_bounding_box_fence_status = glClientWaitSync(fluid_bounding_box_fence, 0u, 0u);
		}
		//auto t_1 = std::chrono::high_resolution_clock::now();
		//std::chrono::duration<double, std::milli> ms = t_1 - t_0;
		//std::cout << ms.count() * 120.0 / 1000.0 << std::endl;

		GLuint changed_fluid_leaf_count;
		std::memcpy
		(
			&changed_fluid_leaf_count, 
			environment.state.GPU_buffers.fluid.changed_bounding_boxes.mapping + environment.state.GPU_buffers.fluid.changed_bounding_boxes.size_offset,
			sizeof(GLuint)
		);

		glClearNamedBufferSubData
		(
			environment.state.GPU_buffers.fluid.changed_bounding_boxes.buffer,
			GL_R32UI,
			environment.state.GPU_buffers.fluid.changed_bounding_boxes.size_offset, sizeof(GLuint),
			GL_RED_INTEGER, GL_UNSIGNED_INT,
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
			unsigned char const* index_start{ environment.state.GPU_buffers.fluid.changed_bounding_boxes.mapping + environment.state.GPU_buffers.fluid.changed_bounding_boxes.boxes_index_offset };
			unsigned char const* min_x_start{ environment.state.GPU_buffers.fluid.changed_bounding_boxes.mapping + environment.state.GPU_buffers.fluid.changed_bounding_boxes.boxes_min_x_offset };
			unsigned char const* min_y_start{ environment.state.GPU_buffers.fluid.changed_bounding_boxes.mapping + environment.state.GPU_buffers.fluid.changed_bounding_boxes.boxes_min_y_offset };
			unsigned char const* max_x_start{ environment.state.GPU_buffers.fluid.changed_bounding_boxes.mapping + environment.state.GPU_buffers.fluid.changed_bounding_boxes.boxes_max_x_offset };
			unsigned char const* max_y_start{ environment.state.GPU_buffers.fluid.changed_bounding_boxes.mapping + environment.state.GPU_buffers.fluid.changed_bounding_boxes.boxes_max_y_offset };

			for (GLuint i{ 0u }; i < changed_fluid_leaf_count; ++i)
			{
				GLuint index;
				GLint min_x, min_y, max_x, max_y;
				std::memcpy(&index, index_start, sizeof(GLuint));
				std::memcpy(&min_x, min_x_start, sizeof(GLint));
				std::memcpy(&min_y, min_y_start, sizeof(GLint));
				std::memcpy(&max_x, max_x_start, sizeof(GLint));
				std::memcpy(&max_y, max_y_start, sizeof(GLint));

				util::proximity::change_leaf_of_multinode_tree
				(
					environment.state.proximity_tree, game_logic_MAX_LEAF_COUNT(environment),
					index, i, min_x, min_y, max_x, max_y
				);

				index_start += environment.state.GPU_buffers.rigid_bodies.triangles.changed_bounding_boxes.boxes_stride;
				min_x_start += environment.state.GPU_buffers.rigid_bodies.triangles.changed_bounding_boxes.boxes_stride;
				min_y_start += environment.state.GPU_buffers.rigid_bodies.triangles.changed_bounding_boxes.boxes_stride;
				max_x_start += environment.state.GPU_buffers.rigid_bodies.triangles.changed_bounding_boxes.boxes_stride;
				max_y_start += environment.state.GPU_buffers.rigid_bodies.triangles.changed_bounding_boxes.boxes_stride;

				//std::cout << i << ": " << index << std::endl;
			}
		}

		GLenum triangle_bounding_box_fence_status = glClientWaitSync(triangle_bounding_box_fence, 0u, 0u);
		while (triangle_bounding_box_fence_status != GL_ALREADY_SIGNALED && triangle_bounding_box_fence_status != GL_CONDITION_SATISFIED)
		{
			// TODO: Do something useful but not necessary while we wait. 
			// Example: Optimize proximity tree.
			triangle_bounding_box_fence_status = glClientWaitSync(triangle_bounding_box_fence, 0u, 0u);
		}

		GLuint changed_triangle_leaf_count;
		std::memcpy
		(
			&changed_triangle_leaf_count,
			environment.state.changed_bounding_boxes_mapping + environment.state.GPU_buffers.rigid_bodies.triangles.changed_bounding_boxes.size_offset,
			sizeof(GLuint)
		);

		glClearNamedBufferSubData
		(
			environment.state.GPU_buffers.rigid_bodies.triangles.changed_bounding_boxes.buffer,
			GL_R32UI,
			environment.state.GPU_buffers.rigid_bodies.triangles.changed_bounding_boxes.size_offset, sizeof(GLuint),
			GL_RED_INTEGER, GL_UNSIGNED_INT,
			nullptr
		);

		environment.state.proximity_tree.changed_leaf_count = changed_fluid_leaf_count + changed_triangle_leaf_count;

		if (util::proximity::is_empty(environment.state.proximity_tree))
		{

		}
		else if (util::proximity::has_single_node(environment.state.proximity_tree, game_logic_MAX_LEAF_COUNT(environment)))
		{

		}
		else
		{
			unsigned char const* index_start{ environment.state.changed_bounding_boxes_mapping + environment.state.GPU_buffers.rigid_bodies.triangles.changed_bounding_boxes.boxes_index_offset };
			unsigned char const* min_x_start{ environment.state.changed_bounding_boxes_mapping + environment.state.GPU_buffers.rigid_bodies.triangles.changed_bounding_boxes.boxes_min_x_offset };
			unsigned char const* min_y_start{ environment.state.changed_bounding_boxes_mapping + environment.state.GPU_buffers.rigid_bodies.triangles.changed_bounding_boxes.boxes_min_y_offset };
			unsigned char const* max_x_start{ environment.state.changed_bounding_boxes_mapping + environment.state.GPU_buffers.rigid_bodies.triangles.changed_bounding_boxes.boxes_max_x_offset };
			unsigned char const* max_y_start{ environment.state.changed_bounding_boxes_mapping + environment.state.GPU_buffers.rigid_bodies.triangles.changed_bounding_boxes.boxes_max_y_offset };

			for (GLuint i{ changed_fluid_leaf_count }; i < environment.state.proximity_tree.changed_leaf_count; ++i)
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

				index_start += environment.state.GPU_buffers.rigid_bodies.triangles.changed_bounding_boxes.boxes_stride;
				min_x_start += environment.state.GPU_buffers.rigid_bodies.triangles.changed_bounding_boxes.boxes_stride;
				min_y_start += environment.state.GPU_buffers.rigid_bodies.triangles.changed_bounding_boxes.boxes_stride;
				max_x_start += environment.state.GPU_buffers.rigid_bodies.triangles.changed_bounding_boxes.boxes_stride;
				max_y_start += environment.state.GPU_buffers.rigid_bodies.triangles.changed_bounding_boxes.boxes_stride;

				//std::cout << i << ": " << index << std::endl;
			}

			GLuint const old_fluid_contact_count{ environment.state.GPU_buffers.fluid.contact_count.current_contact_count };
			GLuint const old_triangle_contact_count{ environment.state.current_triangle_contact_count };

			glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT); // Updated persistent contacts
			
			{	// Fluid contact remove
				struct On_Removing_Contacts_For
				{
					void operator()(GLuint leaf_index)
					{
					}
				};

				struct Remove_Contact
				{
					game_environment::Environment& environment;

					void operator()(GLuint const contact_index, GLuint& next_contact_index)
					{
						if (contact_index < MAX_FLUID_CONTACT_COUNT(environment))
						{
							--environment.state.GPU_buffers.fluid.contact_count.current_contact_count;
							if (contact_index != environment.state.GPU_buffers.fluid.contact_count.current_contact_count)
							{
								glCopyNamedBufferSubData
								(
									environment.state.GPU_buffers.fluid.contacts.buffer, environment.state.GPU_buffers.fluid.contacts.buffer,
									environment.state.GPU_buffers.fluid.contacts.contacts_offset + environment.state.GPU_buffers.fluid.contact_count.current_contact_count * environment.state.GPU_buffers.fluid.contacts.contacts_stride,
									environment.state.GPU_buffers.fluid.contacts.contacts_offset + contact_index * environment.state.GPU_buffers.fluid.contacts.contacts_stride,
									environment.state.GPU_buffers.fluid.contacts.contacts_stride
								);

								util::proximity::move_contact
								(
									environment.state.proximity_tree,
									environment.state.GPU_buffers.fluid.contact_count.current_contact_count, contact_index,
									next_contact_index
								);
							}
						}
						else
						{
							GLuint const fluid_triangle_contact_index{ contact_index - MAX_FLUID_CONTACT_COUNT(environment) };
							--environment.state.GPU_buffers.fluid_triangle.contact_count.current_contact_count;
							if (fluid_triangle_contact_index != environment.state.GPU_buffers.fluid_triangle.contact_count.current_contact_count)
							{
								glCopyNamedBufferSubData
								(
									environment.state.GPU_buffers.fluid_triangle.contacts.buffer, environment.state.GPU_buffers.fluid_triangle.contacts.buffer,
									environment.state.GPU_buffers.fluid_triangle.contacts.contacts_offset + environment.state.GPU_buffers.fluid_triangle.contact_count.current_contact_count * environment.state.GPU_buffers.fluid_triangle.contacts.contacts_stride,
									environment.state.GPU_buffers.fluid_triangle.contacts.contacts_offset + fluid_triangle_contact_index * environment.state.GPU_buffers.fluid_triangle.contacts.contacts_stride,
									environment.state.GPU_buffers.fluid_triangle.contacts.contacts_stride
								);

								util::proximity::move_contact
								(
									environment.state.proximity_tree,
									MAX_FLUID_CONTACT_COUNT(environment) + environment.state.GPU_buffers.fluid_triangle.contact_count.current_contact_count, contact_index,
									next_contact_index
								);
							}
						}
					}
				};
				On_Removing_Contacts_For on_removing_contacts_for{};
				Remove_Contact remove_contact{ environment };
				util::proximity::remove_contacts
				(
					environment.state.proximity_tree, game_logic_MAX_LEAF_COUNT(environment),
					0u, changed_fluid_leaf_count,
					on_removing_contacts_for, remove_contact
				);
			}
			{	// Triangle contact remove
				struct On_Removing_Contacts_For
				{
					void operator()(GLuint leaf_index)
					{
					}
				};

				struct Remove_Contact
				{
					game_environment::Environment& environment;

					void operator()(GLuint const contact_index, GLuint& next_contact_index)
					{
						if (contact_index < TRIANGLE_CONTACT_BASE_INDEX(environment))
						{
							GLuint const fluid_triangle_contact_index{ contact_index - MAX_FLUID_CONTACT_COUNT(environment) };
							--environment.state.GPU_buffers.fluid_triangle.contact_count.current_contact_count;
							if (fluid_triangle_contact_index != environment.state.GPU_buffers.fluid_triangle.contact_count.current_contact_count)
							{
								glCopyNamedBufferSubData
								(
									environment.state.GPU_buffers.fluid_triangle.contacts.buffer, environment.state.GPU_buffers.fluid_triangle.contacts.buffer,
									environment.state.GPU_buffers.fluid_triangle.contacts.contacts_offset + environment.state.GPU_buffers.fluid_triangle.contact_count.current_contact_count * environment.state.GPU_buffers.fluid_triangle.contacts.contacts_stride,
									environment.state.GPU_buffers.fluid_triangle.contacts.contacts_offset + fluid_triangle_contact_index * environment.state.GPU_buffers.fluid_triangle.contacts.contacts_stride,
									environment.state.GPU_buffers.fluid_triangle.contacts.contacts_stride
								);

								util::proximity::move_contact
								(
									environment.state.proximity_tree, 
									MAX_FLUID_CONTACT_COUNT(environment) + environment.state.GPU_buffers.fluid_triangle.contact_count.current_contact_count, contact_index,
									next_contact_index
								);
							}
						}
						else
						{
							GLuint const triangle_contact_index{ contact_index - TRIANGLE_CONTACT_BASE_INDEX(environment) };
							--environment.state.current_triangle_contact_count;
							if (triangle_contact_index != environment.state.current_triangle_contact_count)
							{
								// MAYBE TODO: Avoid multiplications by using members that are added/subtracted along with contact count
								glCopyNamedBufferSubData
								(
									environment.state.contact_buffer, environment.state.contact_buffer,
									environment.state.contact_buffer_contacts_offset + environment.state.current_triangle_contact_count * environment.state.contact_buffer_contacts_stride,
									environment.state.contact_buffer_contacts_offset + triangle_contact_index * environment.state.contact_buffer_contacts_stride,
									sizeof(GLuint[2])
								);
								glCopyNamedBufferSubData
								(
									environment.state.contact_surface_buffer, environment.state.contact_surface_buffer,
									environment.state.contact_surface_buffer_contact_surfaces_offset + environment.state.current_triangle_contact_count * environment.state.contact_surface_buffer_contact_surfaces_stride,
									environment.state.contact_surface_buffer_contact_surfaces_offset + triangle_contact_index * environment.state.contact_surface_buffer_contact_surfaces_stride,
									environment.state.contact_surface_buffer_contact_surfaces_stride
								);

								util::proximity::move_contact
								(
									environment.state.proximity_tree, 
									TRIANGLE_CONTACT_BASE_INDEX(environment) + environment.state.current_triangle_contact_count, contact_index, 
									next_contact_index
								);
							}
						}
					}
				};
				On_Removing_Contacts_For on_removing_contacts_for{};
				Remove_Contact remove_contact{ environment };
				util::proximity::remove_contacts
				(
					environment.state.proximity_tree, game_logic_MAX_LEAF_COUNT(environment),
					changed_fluid_leaf_count, environment.state.proximity_tree.changed_leaf_count,	// TODO: changed leaf count can be a local variable
					on_removing_contacts_for, remove_contact
				);
			}

			// TODO: Check if this memory barrier is necessary
			// TODO: This barrier is not necessary if we only use glClearNamedBufferSubData for uploading 
			// new contacts. Is that the best way though?
			glMemoryBarrier(GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT);
			GLsync const fence{ glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0u) };	// Contact copying
			glFlush();

			/*glClearNamedBufferSubData
			(
				environment.state.fluid_contact_count_buffer,
				GL_R32UI,
				environment.state.fluid_contact_count_buffer_persistent_count_offset, sizeof(GLuint),
				GL_RED_INTEGER, GL_UNSIGNED_INT, 
				&environment.state.current_fluid_contact_count
			);*/

			environment.state.GPU_buffers.fluid.contact_count.current_persistent_contact_count = environment.state.GPU_buffers.fluid.contact_count.current_contact_count;

			environment.state.GPU_buffers.fluid_triangle.contact_count.current_persistent_contact_count = environment.state.GPU_buffers.fluid_triangle.contact_count.current_contact_count;

			glClearNamedBufferSubData
			(
				environment.state.GPU_buffers.rigid_bodies.triangles.persistent_contact_count.buffer,
				GL_R32UI,
				environment.state.GPU_buffers.rigid_bodies.triangles.persistent_contact_count.persistent_contact_count_offset, sizeof(GLuint),
				GL_RED_INTEGER, GL_UNSIGNED_INT,
				&environment.state.current_triangle_contact_count
			);

			// TODO: Give GPU something heavy to do while we write the new contacts

			environment.state.current_persistent_contact_count = environment.state.current_triangle_contact_count;

			// TODO: Other operations that we let the GPU perform while it is copying contact 
			// to old contact positions.

			// TODO: Potential one-time CPU operations we must do before 
			// waiting for the GPU to be done. Example: Find the first 
			// new contact before waiting.

			// IMPORTANT TODO: Collect new contacts in temporary array while waiting for GPU
			// ALTERNATIVE: Write to separate buffer that is then copied.

			GLenum fence_status = glClientWaitSync(fence, 0u, 0u);
			while (fence_status != GL_ALREADY_SIGNALED && fence_status != GL_CONDITION_SATISFIED)
			{
				// TODO: Do something useful but not necessary while we wait. 
				// Example: Optimize proximity tree.
				fence_status = glClientWaitSync(fence, 0u, 0u);
			}

			{ // Fluid contact add
				struct On_Adding_Contacts_For
				{
					void operator()(GLuint leaf_index)
					{
					}
				};

				struct Contact_Can_Be_Added
				{
					game_environment::Environment const& environment;

					bool operator()(GLuint const leaf_0_index, GLuint const leaf_1_index)
					{
						// IMPORTANT TODO: STORE THE TYPE OF LEAF 1
						if (leaf_1_index < game_logic_MAX_FLUID_PARTICLE_COUNT(environment))
						{
							return environment.state.GPU_buffers.fluid.contact_count.current_contact_count < MAX_FLUID_CONTACT_COUNT(environment);
						}
						return environment.state.GPU_buffers.fluid_triangle.contact_count.current_contact_count < MAX_FLUID_TRIANGLE_CONTACT_COUNT(environment);
						/*return
						(
							environment.state.current_contact_count < game_logic_MAX_TRIANGLE_CONTACT_COUNT(environment)// &&
							//leaf_0_index >= game_logic_TRIANGLE_LEAFS_BASE_INDEX(environment) &&
							//leaf_1_index >= game_logic_TRIANGLE_LEAFS_BASE_INDEX(environment)
						);*/
					}
				};

				struct Add_Contact
				{
					game_environment::Environment& environment;

					GLuint operator()(GLuint const leaf_0_index, GLuint const leaf_1_index)
					{
						if (leaf_1_index < game_logic_MAX_FLUID_PARTICLE_COUNT(environment))
						{
							GLuint leaf_indices[2u]{
								leaf_0_index,
								leaf_1_index
							};
							// TODO: Do addition between mapping pointer and offset once during initialization
							/*std::memcpy
							(
								environment.state.fluid_contact_mapping + environment.state.fluid_contact_buffer_contacts_particles_offset
								+ environment.state.current_fluid_contact_count * environment.state.fluid_contact_buffer_contacts_stride,
								leaf_indices, sizeof(leaf_indices)
							);*/
							glClearNamedBufferSubData
							(
								environment.state.GPU_buffers.fluid.contacts.buffer,
								GL_RG32UI,
								environment.state.GPU_buffers.fluid.contacts.contacts_particles_offset
								+ environment.state.GPU_buffers.fluid.contact_count.current_contact_count * environment.state.GPU_buffers.fluid.contacts.contacts_stride,
								sizeof(leaf_indices),
								GL_RG_INTEGER, GL_UNSIGNED_INT,
								leaf_indices
							);

							return environment.state.GPU_buffers.fluid.contact_count.current_contact_count++;
						}
						GLuint contact_key[2u]{
							leaf_0_index, 
							leaf_1_index - game_logic_TRIANGLE_LEAFS_BASE_INDEX(environment) // IMPORTANT TODO: DO SUBTRACTION ON GPU
						};
						// IMPORTANT TODO: Combine to single clear call
						glClearNamedBufferSubData
						(
							environment.state.GPU_buffers.fluid_triangle.contacts.buffer,
							GL_R32UI,
							environment.state.GPU_buffers.fluid_triangle.contacts.contacts_triangle_offset
							+ environment.state.GPU_buffers.fluid_triangle.contact_count.current_contact_count * environment.state.GPU_buffers.fluid_triangle.contacts.contacts_stride,
							sizeof(GLuint),
							GL_RED_INTEGER, GL_UNSIGNED_INT,
							&contact_key[1u]
						);
						glClearNamedBufferSubData
						(
							environment.state.GPU_buffers.fluid_triangle.contacts.buffer,
							GL_R32UI,
							environment.state.GPU_buffers.fluid_triangle.contacts.contacts_particle_offset
							+ environment.state.GPU_buffers.fluid_triangle.contact_count.current_contact_count * environment.state.GPU_buffers.fluid_triangle.contacts.contacts_stride,
							sizeof(GLuint),
							GL_RED_INTEGER, GL_UNSIGNED_INT,
							&contact_key[0u]
						);
						return MAX_FLUID_CONTACT_COUNT(environment) + environment.state.GPU_buffers.fluid_triangle.contact_count.current_contact_count++;
						/*// TODO: Check which type of contact it is
						// IMPORTANT TODO: Do the base index subtraction in the new contact shaders
						GLuint leaf_indices[2u]{
							leaf_0_index - game_logic_TRIANGLE_LEAFS_BASE_INDEX(environment),
							leaf_1_index - game_logic_TRIANGLE_LEAFS_BASE_INDEX(environment)
						};
						// TODO: Do addition between mapping pointer and offset once during initialization
						std::memcpy
						(
							environment.state.contact_mapping + environment.state.contact_buffer_contacts_offset
							+ environment.state.current_triangle_contact_count * environment.state.contact_buffer_contacts_stride,
							&leaf_indices,
							sizeof(leaf_indices)
						);
						GLuint const contact_index = TRIANGLE_CONTACT_BASE_INDEX(environment) + environment.state.current_triangle_contact_count;
						++environment.state.current_triangle_contact_count;
						return contact_index;*/
					}
				};

				On_Adding_Contacts_For on_adding_contacts_for{};
				Contact_Can_Be_Added contact_can_be_added{ environment };
				Add_Contact add_contact{ environment };

				util::proximity::add_contacts
				(
					environment.state.proximity_tree, game_logic_MAX_LEAF_COUNT(environment),
					0u, changed_fluid_leaf_count,
					on_adding_contacts_for, contact_can_be_added, add_contact
				);
			}

			GLuint const new_fluid_contact_count{environment.state.GPU_buffers.fluid.contact_count.current_contact_count - environment.state.GPU_buffers.fluid.contact_count.current_persistent_contact_count};
			//std::cout << new_fluid_contact_count << std::endl;
			/*glFlushMappedNamedBufferRange
			(
				environment.state.GPU_buffers.fluid.contacts.buffer, 
				environment.state.fluid_contact_buffer_contacts_offset + environment.state.current_fluid_persistent_contact_count * environment.state.fluid_contact_buffer_contacts_stride,
				new_fluid_contact_count * environment.state.fluid_contact_buffer_contacts_stride
			);*/
			glClearNamedBufferSubData
			(
				environment.state.GPU_buffers.fluid.contact_count.buffer,
				GL_R32UI,
				environment.state.GPU_buffers.fluid.contact_count.count_offset, sizeof(GLuint),
				GL_RED_INTEGER, GL_UNSIGNED_INT,
				&environment.state.GPU_buffers.fluid.contact_count.current_contact_count
			);
			glUseProgram(environment.state.new_fluid_contacts_shader);
			glUniform1ui(environment.state.new_fluid_contacts_shader_persistent_count_uniform_location, environment.state.GPU_buffers.fluid.contact_count.current_persistent_contact_count);
			glDispatchCompute
			(
				ceil_div(new_fluid_contact_count, NEW_FLUID_CONTACT_LOCAL_SIZE(environment)),
				1u, 1u
			);

			{	// Triangle contact add
				struct On_Adding_Contacts_For
				{
					void operator()(GLuint leaf_index)
					{
					}
				};

				struct Contact_Can_Be_Added
				{
					game_environment::Environment const& environment;

					bool operator()(GLuint const leaf_0_index, GLuint const leaf_1_index)
					{
						if (leaf_1_index < game_logic_MAX_FLUID_PARTICLE_COUNT(environment))
						{
							return environment.state.GPU_buffers.fluid_triangle.contact_count.current_contact_count < MAX_FLUID_TRIANGLE_CONTACT_COUNT(environment);
						}
						return environment.state.current_triangle_contact_count < game_logic_MAX_TRIANGLE_CONTACT_COUNT(environment);
						/*
						return
							(
								environment.state.current_triangle_contact_count < game_logic_MAX_TRIANGLE_CONTACT_COUNT(environment) &&
								leaf_0_index >= game_logic_TRIANGLE_LEAFS_BASE_INDEX(environment) &&
								leaf_1_index >= game_logic_TRIANGLE_LEAFS_BASE_INDEX(environment)
								);*/
					}
				};

				struct Add_Contact
				{
					game_environment::Environment& environment;

					GLuint operator()(GLuint const leaf_0_index, GLuint const leaf_1_index)
					{
						if (leaf_1_index < game_logic_MAX_FLUID_PARTICLE_COUNT(environment))
						{
							GLuint contact_key[2u]{
								leaf_1_index,
								leaf_0_index - game_logic_TRIANGLE_LEAFS_BASE_INDEX(environment) // IMPORTANT TODO: DO SUBTRACTION ON GPU
							};
							// IMPORTANT TODO: Combine to single clear call
							glClearNamedBufferSubData
							(
								environment.state.GPU_buffers.fluid_triangle.contacts.buffer,
								GL_R32UI,
								environment.state.GPU_buffers.fluid_triangle.contacts.contacts_triangle_offset
								+ environment.state.GPU_buffers.fluid_triangle.contact_count.current_contact_count * environment.state.GPU_buffers.fluid_triangle.contacts.contacts_stride,
								sizeof(GLuint),
								GL_RED_INTEGER, GL_UNSIGNED_INT,
								&contact_key[1u]
							);
							glClearNamedBufferSubData
							(
								environment.state.GPU_buffers.fluid_triangle.contacts.buffer,
								GL_R32UI,
								environment.state.GPU_buffers.fluid_triangle.contacts.contacts_particle_offset
								+ environment.state.GPU_buffers.fluid_triangle.contact_count.current_contact_count * environment.state.GPU_buffers.fluid_triangle.contacts.contacts_stride,
								sizeof(GLuint),
								GL_RED_INTEGER, GL_UNSIGNED_INT,
								&contact_key[0u]
							);
							return MAX_FLUID_CONTACT_COUNT(environment) + environment.state.GPU_buffers.fluid_triangle.contact_count.current_contact_count++;
							//return environment.state.GPU_buffers.fluid_triangle.contact_count.current_contact_count++;
						}

						// IMPORTANT TODO: Do the base index subtraction in the new contact shaders
						GLuint leaf_indices[2u]{
							leaf_0_index - game_logic_TRIANGLE_LEAFS_BASE_INDEX(environment),
							leaf_1_index - game_logic_TRIANGLE_LEAFS_BASE_INDEX(environment)
						};
						// TODO: Do addition between mapping pointer and offset once during initialization
						/*std::memcpy
						(
							environment.state.contact_mapping + environment.state.contact_buffer_contacts_offset
							+ environment.state.current_triangle_contact_count * environment.state.contact_buffer_contacts_stride,
							&leaf_indices,
							sizeof(leaf_indices)
						);*/
						glClearNamedBufferSubData
						(
							environment.state.contact_buffer, 
							GL_RG32UI, 
							environment.state.contact_buffer_contacts_offset 
							+ environment.state.current_triangle_contact_count * environment.state.contact_buffer_contacts_stride, 
							sizeof(leaf_indices), 
							GL_RG_INTEGER, GL_UNSIGNED_INT, 
							leaf_indices
						);
						return TRIANGLE_CONTACT_BASE_INDEX(environment) + environment.state.current_triangle_contact_count++;
					}
				};

				On_Adding_Contacts_For on_adding_contacts_for{};
				Contact_Can_Be_Added contact_can_be_added{ environment };
				Add_Contact add_contact{ environment };
				util::proximity::add_contacts
				(
					environment.state.proximity_tree, game_logic_MAX_LEAF_COUNT(environment),
					changed_fluid_leaf_count, environment.state.proximity_tree.changed_leaf_count,
					on_adding_contacts_for, contact_can_be_added, add_contact
				);
			}

			GLuint const added_fluid_triangle_contacts_count{ environment.state.GPU_buffers.fluid_triangle.contact_count.current_contact_count - environment.state.GPU_buffers.fluid_triangle.contact_count.current_persistent_contact_count };
			glClearNamedBufferSubData
			(
				environment.state.GPU_buffers.fluid_triangle.contact_count.buffer,
				GL_R32UI,
				environment.state.GPU_buffers.fluid_triangle.contact_count.count_offset, sizeof(GLuint),
				GL_RED_INTEGER, GL_UNSIGNED_INT,
				&environment.state.GPU_buffers.fluid_triangle.contact_count.current_contact_count
			);
			glUseProgram(environment.state.new_fluid_triangle_contacts_shader);
			glUniform1ui
			(
				environment.state.new_fluid_triangle_contacts_shader_persistent_count_uniform_location,
				environment.state.GPU_buffers.fluid_triangle.contact_count.current_persistent_contact_count
			);
			glDispatchCompute
			(
				ceil_div(added_fluid_triangle_contacts_count, NEW_FLUID_TRIANGLE_CONTACT_LOCAL_SIZE(environment)), 
				1u, 1u
			);

			GLuint const new_contact_count{ environment.state.current_triangle_contact_count - environment.state.current_persistent_contact_count };
			/*glFlushMappedNamedBufferRange
			(
				environment.state.contact_buffer,
				environment.state.contact_buffer_contacts_offset + environment.state.current_persistent_contact_count * environment.state.contact_buffer_contacts_stride,
				new_contact_count * environment.state.contact_buffer_contacts_stride
			);*/
			glClearNamedBufferSubData
			(
				environment.state.GPU_buffers.rigid_bodies.triangles.contact_count.buffer,
				GL_R32UI,
				environment.state.GPU_buffers.rigid_bodies.triangles.contact_count.contact_count_offset, sizeof(GLuint),
				GL_RED_INTEGER, GL_UNSIGNED_INT,
				&environment.state.current_triangle_contact_count
			);
			glUseProgram(environment.state.new_triangle_contact_shader);
			glDispatchCompute
			(
				ceil_div(new_contact_count, game_logic__util__rigid_body_NEW_TRIANGLE_CONTACT_LOCAL_SIZE(environment)),
				1u, 1u
			);

			/*if (environment.state.tick % 120u == 0u)
			{
				//GLuint count;
				//glGetNamedBufferSubData(environment.state.fluid_contact_count_buffer, environment.state.fluid_contact_count_buffer_persistent_count_offset, sizeof(GLuint), &count);
				//std::cout << environment.state.current_fluid_persistent_contact_count << " = " << count << std::endl;

				std::cout << "Height: " << util::proximity::compute_height
				(
					environment.state.proximity_tree, game_logic_MAX_LEAF_COUNT(environment)
				) << '\n';
				std::cout << "Changed leaf count: " << environment.state.proximity_tree.changed_leaf_count << '\n';
				std::cout << "Fluid contact count: " << old_fluid_contact_count << " - " << old_fluid_contact_count - environment.state.current_fluid_persistent_contact_count << " + " << environment.state.current_fluid_contact_count - environment.state.current_fluid_persistent_contact_count << " = " << environment.state.current_fluid_contact_count << '\n';
				std::cout << "Triangle contact count: " << old_triangle_contact_count << " - " << old_triangle_contact_count - environment.state.current_persistent_contact_count << " + " << environment.state.current_triangle_contact_count - environment.state.current_persistent_contact_count << " = " << environment.state.current_triangle_contact_count << '\n';
				std::cout << std::endl;
			}*/
		}
	}

	/*void tick_positions(game_environment::Environment& environment)
	{
		integrate_velocities(environment);
		update_constraints(environment);
	}*/

	void tick_physics(game_environment::Environment& environment)
	{
		GLint time_elapsed_query_done;
		glGetQueryObjectiv(environment.state.time_elapsed_query, GL_QUERY_RESULT_AVAILABLE, &time_elapsed_query_done);
		if (time_elapsed_query_done == GL_TRUE)
		{
			glBeginQuery(GL_TIME_ELAPSED, environment.state.time_elapsed_query);
		}
		tick_velocities(environment);

		if (time_elapsed_query_done == GL_TRUE)
		{
			glEndQuery(GL_TIME_ELAPSED);
		}

		tick_positions(environment);

		glMemoryBarrier(GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT);
		environment.state.physics_tick_results_fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0u);
		glFlush();
	}

	void on_framebuffer_size_changed(
		game_environment::Environment& environment,
		int width, int height
	)
	{
		delete_fluid_texture_and_framebuffer(environment);
		adapt_to_default_framebuffer_size(environment, width, height);
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
			case GLFW_KEY_SPACE:
				environment.state.physics_running = !environment.state.physics_running;
				break;
			case GLFW_KEY_ENTER:
				tick_physics(environment);
				break;
			case GLFW_KEY_F:
				environment.state.debug_fluid_particles_visible = !environment.state.debug_fluid_particles_visible;
				break;
			case GLFW_KEY_W:
				environment.state.triangle_wireframes_visible = !environment.state.triangle_wireframes_visible;
				break;
			case GLFW_KEY_R:
				environment.state.rigid_bodies_visible = !environment.state.rigid_bodies_visible;
				break;
			case GLFW_KEY_N:
				environment.state.triangle_normals_visible = !environment.state.triangle_normals_visible;
				break;
			case GLFW_KEY_B:
				environment.state.leaf_bounding_boxes_visible = !environment.state.leaf_bounding_boxes_visible;
				break;
			case GLFW_KEY_P:
				environment.state.parent_bounding_boxes_visible = !environment.state.parent_bounding_boxes_visible;
				break;
			case GLFW_KEY_L:
				environment.state.leaf_contacts_visible = !environment.state.leaf_contacts_visible;
				break;
			case GLFW_KEY_C:
				environment.state.contact_point_positions_visible = !environment.state.contact_point_positions_visible;
				break;
			case GLFW_KEY_T:
				environment.state.contact_basis_visible = !environment.state.contact_basis_visible;
				break;
			case GLFW_KEY_I:
				environment.state.contact_impulses_visible = !environment.state.contact_impulses_visible;
				break;
			case GLFW_KEY_G:
				environment.state.gravity_visible = !environment.state.gravity_visible;
				break;
			case GLFW_KEY_BACKSPACE:
				environment.state.debug_fluid_particles_visible = false;
				environment.state.triangle_wireframes_visible = false;
				environment.state.rigid_bodies_visible = false;
				environment.state.triangle_normals_visible = false;
				environment.state.leaf_bounding_boxes_visible = false;
				environment.state.parent_bounding_boxes_visible = false;
				environment.state.leaf_contacts_visible = false;
				environment.state.contact_point_positions_visible = false;
				environment.state.contact_basis_visible = false;
				environment.state.contact_impulses_visible = false;
				environment.state.gravity_visible = false;
				break;
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

	bool triangle_contains_point
	(
		game_environment::Environment const& environment, GLuint const triangle_index,
		GLint x, GLint y, GLfloat& local_x, GLfloat& local_y
	)
	{
		game_state::rigid_body::Triangle const& triangle{ environment.state.GPU_buffers.rigid_bodies.triangles.values[triangle_index] };
		util::rigid_body::Position body_position;
		std::memcpy
		(
			&body_position,
			environment.state.GPU_buffers.rigid_bodies.positions.mapping + environment.state.GPU_buffers.rigid_bodies.positions.p_offset + triangle.body * environment.state.GPU_buffers.rigid_bodies.positions.p_stride,
			sizeof(body_position)
		);
		local_x = static_cast<GLfloat>(x - body_position.position.x);
		local_y = static_cast<GLfloat>(y - body_position.position.y);

		// TODO: Do not loose precision for large angles
		GLfloat angle = game_logic__util__spatial_TO_RADIANS(environment, body_position.angle);
		GLfloat right_x = std::cos(angle);
		GLfloat right_y = std::sin(angle);

		GLfloat const old_local_x{ local_x };
		local_x = local_x * right_x + local_y * right_y;
		local_y = local_y * right_x - old_local_x * right_y;

		GLfloat vertex_0_x{ environment.state.GPU_buffers.rigid_bodies.triangles.vertices.values[triangle.vertices[0u]][0u] };
		GLfloat vertex_0_y{ environment.state.GPU_buffers.rigid_bodies.triangles.vertices.values[triangle.vertices[0u]][1u] };

		GLfloat vertex_1_x{ environment.state.GPU_buffers.rigid_bodies.triangles.vertices.values[triangle.vertices[1u]][0u] };
		GLfloat vertex_1_y{ environment.state.GPU_buffers.rigid_bodies.triangles.vertices.values[triangle.vertices[1u]][1u] };

		GLfloat vertex_2_x{ environment.state.GPU_buffers.rigid_bodies.triangles.vertices.values[triangle.vertices[2u]][0u] };
		GLfloat vertex_2_y{ environment.state.GPU_buffers.rigid_bodies.triangles.vertices.values[triangle.vertices[2u]][1u] };

		GLfloat normal_0_x = vertex_1_y - vertex_0_y;
		GLfloat normal_0_y = vertex_0_x - vertex_1_x;

		GLfloat normal_1_x = vertex_2_y - vertex_1_y;
		GLfloat normal_1_y = vertex_1_x - vertex_2_x;

		GLfloat normal_2_x = vertex_0_y - vertex_2_y;
		GLfloat normal_2_y = vertex_2_x - vertex_0_x;

		return
		(
			local_x * normal_0_x + local_y * normal_0_y <= vertex_0_x * normal_0_x + vertex_0_y * normal_0_y &&
			local_x * normal_1_x + local_y * normal_1_y <= vertex_1_x * normal_1_x + vertex_1_y * normal_1_y &&
			local_x * normal_2_x + local_y * normal_2_y <= vertex_2_x * normal_2_x + vertex_2_y * normal_2_y
		);
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
		case GLFW_MOUSE_BUTTON_LEFT:
			switch (action)
			{
			case GLFW_PRESS:
				if (mods & GLFW_MOD_CONTROL)
				{
					GLint cursor_world_position[2u];
					window_to_world::window_screen_cursor_position_to_world_position
					(
						environment,
						&cursor_world_position[0u], &cursor_world_position[1u]
					);
					environment.state.gravity_source_positions[environment.state.current_gravity_source_count] = ::util::math::Vector_2D{ cursor_world_position[0u], cursor_world_position[1u] };
					environment.state.gravity_source_strengths[environment.state.current_gravity_source_count] = 0.0f;
					glClearNamedBufferSubData
					(
						environment.state.gravity_sources_buffer,
						GL_RG32UI,
						environment.state.gravity_sources_buffer_positions_offset + environment.state.current_gravity_source_count * environment.state.gravity_sources_buffer_positions_stride,
						sizeof(GLuint[2u]),
						GL_RG_INTEGER, GL_UNSIGNED_INT,
						cursor_world_position
					);
					glClearNamedBufferSubData
					(
						environment.state.gravity_sources_buffer,
						GL_R32F,
						environment.state.gravity_sources_buffer_strengths_offset + environment.state.current_gravity_source_count * environment.state.gravity_sources_buffer_strengths_stride,
						sizeof(GLfloat),
						GL_RED, GL_FLOAT,
						nullptr
					);
					++environment.state.current_gravity_source_count;
					glClearNamedBufferSubData
					(
						environment.state.gravity_sources_buffer, 
						GL_R32UI, 
						environment.state.gravity_sources_buffer_count_offset, 
						sizeof(GLuint), 
						GL_RED_INTEGER, GL_UNSIGNED_INT, 
						&environment.state.current_gravity_source_count
					);
					return;
				}

				if (!environment.state.point_grabbed)
				{
					GLint cursor_world_x, cursor_world_y;
					window_to_world::window_screen_cursor_position_to_world_position
					(
						environment,
						&cursor_world_x, &cursor_world_y
					);

					GLuint hovered_gravity_source;
					for (hovered_gravity_source = 0u; hovered_gravity_source < environment.state.current_gravity_source_count; ++hovered_gravity_source)
					{
						::util::math::Vector_2D const& gravity_source_position{ environment.state.gravity_source_positions[hovered_gravity_source] };
						GLfloat diff_x{ static_cast<GLfloat>(cursor_world_x - gravity_source_position.x) };
						GLfloat diff_y{ static_cast<GLfloat>(cursor_world_y - gravity_source_position.y) };
						GLfloat distance_squared{ diff_x * diff_x + diff_y * diff_y };
						if (distance_squared <= GRAVITY_SOURCE_GRAB_RADIUS(environment) * GRAVITY_SOURCE_GRAB_RADIUS(environment))
						{
							break;
						}
					}
					if (hovered_gravity_source != environment.state.current_gravity_source_count)
					{
						environment.state.grabbed_gravity_source = hovered_gravity_source;
					}
					else
					{
						GLenum fence_status = glClientWaitSync(environment.state.physics_tick_results_fence, 0u, 0u);
						while (fence_status != GL_ALREADY_SIGNALED && fence_status != GL_CONDITION_SATISFIED)
						{
							// TODO: Do something useful but not necessary while we wait. 
							// Example: Optimize proximity tree.
							fence_status = glClientWaitSync(environment.state.physics_tick_results_fence, 0u, 0u);
						}

						if (util::proximity::is_empty(environment.state.proximity_tree))
						{

						}
						else if (util::proximity::has_single_node(environment.state.proximity_tree, game_logic_MAX_LEAF_COUNT(environment)))
						{

						}
						else
						{
							struct On_Leaf_Found
							{
								game_environment::Environment& environment;
								GLint const cursor_world_x;
								GLint const cursor_world_y;
								GLuint& hovered_triangle;
								GLfloat& hovered_local_x;
								GLfloat& hovered_local_y;

								void operator()(GLuint leaf_index, game_state::proximity::Node const& leaf)
								{
									if (leaf_index < game_logic_TRIANGLE_LEAFS_BASE_INDEX(environment))
									{
										return;
									}
									leaf_index -= game_logic_TRIANGLE_LEAFS_BASE_INDEX(environment);

									game_state::rigid_body::Triangle const& triangle{ environment.state.GPU_buffers.rigid_bodies.triangles.values[leaf_index] };
									GLint body_position[4u];
									std::memcpy
									(
										&body_position,
										environment.state.GPU_buffers.rigid_bodies.positions.mapping + environment.state.GPU_buffers.rigid_bodies.positions.p_offset + triangle.body * environment.state.GPU_buffers.rigid_bodies.positions.p_stride,
										sizeof(body_position)
									);
									GLfloat local_x;
									GLfloat local_y;
									if (
										triangle_contains_point(environment, leaf_index, cursor_world_x, cursor_world_y, local_x, local_y) &&
										leaf_index < hovered_triangle
										)
									{
										hovered_triangle = leaf_index;
										hovered_local_x = local_x;
										hovered_local_y = local_y;
									}
								}
							};
							GLuint hovered_triangle{ game_logic__util__proximity_NULL_INDEX };
							GLfloat hovered_local_x;
							GLfloat hovered_local_y;
							On_Leaf_Found on_leaf_found{ environment, cursor_world_x, cursor_world_y, hovered_triangle, hovered_local_x, hovered_local_y };
							util::proximity::query_point_of_multinode_tree
							(
								environment.state.proximity_tree, game_logic_MAX_LEAF_COUNT(environment),
								cursor_world_x, cursor_world_y,
								on_leaf_found
							);
							if (hovered_triangle != game_logic__util__proximity_NULL_INDEX)
							{
								glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT);

								glClearNamedBufferSubData
								(
									environment.state.cursor_constrained_point_buffer,
									GL_R32UI,
									environment.state.cursor_constrained_point_buffer_body_offset, sizeof(GLuint),
									GL_RED_INTEGER, GL_UNSIGNED_INT,
									&environment.state.GPU_buffers.rigid_bodies.triangles.values[hovered_triangle].body
								);
								// TODO: Put hovered_local_x, hovered_local_y in array from start
								GLfloat local_point[2u]
								{
									game_logic__util__spatial_TO_METERS(environment, hovered_local_x),
									game_logic__util__spatial_TO_METERS(environment, hovered_local_y)
								};
								glClearNamedBufferSubData
								(
									environment.state.cursor_constrained_point_buffer,
									GL_RG32F,
									environment.state.cursor_constrained_point_buffer_local_point_offset, sizeof(local_point),
									GL_RG, GL_FLOAT,
									local_point
								);

								glClearNamedBufferSubData
								(
									environment.state.cursor_constraint_buffer,
									GL_RG32F,
									environment.state.cursor_constraint_buffer_impulse_offset, sizeof(GLfloat[2u]),
									GL_RG, GL_FLOAT,
									nullptr
								);

								environment.state.grabbed_triangle = hovered_triangle;
							}
						}
					}
				}
				break;
			case GLFW_RELEASE:
				environment.state.grabbed_gravity_source = game_logic__util__proximity_NULL_INDEX;
				environment.state.grabbed_triangle = game_logic__util__proximity_NULL_INDEX;
				break;
			}
			break;
		case GLFW_MOUSE_BUTTON_MIDDLE:
			switch (action)
			{ 
			case GLFW_PRESS:
				environment.state.point_grabbed = true;
				window_to_world::window_screen_cursor_position_to_world_position(
					environment,
					&environment.state.grabbed_point.x, &environment.state.grabbed_point.y
				);
				glfwSetCursor(environment.window, environment.state.cursor_types.grab_cursor);
				break; 
			case GLFW_RELEASE:
				environment.state.point_grabbed = false;
				glfwSetCursor(environment.window, nullptr);
				break;
			}
			break;
		case GLFW_MOUSE_BUTTON_RIGHT:
			switch (action)
			{
			case GLFW_PRESS:
				if (!environment.state.point_grabbed)
				{
					GLint cursor_world_x, cursor_world_y;
					window_to_world::window_screen_cursor_position_to_world_position
					(
						environment,
						&cursor_world_x, &cursor_world_y
					);

					GLuint hovered_gravity_source;
					for (hovered_gravity_source = 0u; hovered_gravity_source < environment.state.current_gravity_source_count; ++hovered_gravity_source)
					{
						::util::math::Vector_2D const& gravity_source_position{ environment.state.gravity_source_positions[hovered_gravity_source] };
						GLfloat diff_x{ static_cast<GLfloat>(cursor_world_x - gravity_source_position.x) };
						GLfloat diff_y{ static_cast<GLfloat>(cursor_world_y - gravity_source_position.y) };
						GLfloat distance_squared{ diff_x * diff_x + diff_y * diff_y };
						if (distance_squared <= GRAVITY_SOURCE_GRAB_RADIUS(environment) * GRAVITY_SOURCE_GRAB_RADIUS(environment))
						{
							break;
						}
					}
					if (hovered_gravity_source != environment.state.current_gravity_source_count)
					{
						if (hovered_gravity_source == environment.state.grabbed_gravity_source)
						{
							environment.state.grabbed_gravity_source = game_logic__util__proximity_NULL_INDEX;
						}
						--environment.state.current_gravity_source_count;
						glClearNamedBufferSubData
						(
							environment.state.gravity_sources_buffer, 
							GL_R32UI, 
							environment.state.gravity_sources_buffer_count_offset, 
							sizeof(GLuint), 
							GL_RED_INTEGER, GL_UNSIGNED_INT, 
							&environment.state.current_gravity_source_count
						);
						if (hovered_gravity_source != environment.state.current_gravity_source_count)
						{
							environment.state.gravity_source_positions[hovered_gravity_source] = environment.state.gravity_source_positions[environment.state.current_gravity_source_count];
							environment.state.gravity_source_strengths[hovered_gravity_source] = environment.state.gravity_source_strengths[environment.state.current_gravity_source_count];
							glCopyNamedBufferSubData
							(
								environment.state.gravity_sources_buffer, environment.state.gravity_sources_buffer, 
								environment.state.gravity_sources_buffer_positions_offset + environment.state.current_gravity_source_count * environment.state.gravity_sources_buffer_positions_stride, 
								environment.state.gravity_sources_buffer_positions_offset + hovered_gravity_source * environment.state.gravity_sources_buffer_positions_stride, 
								sizeof(GLint[2u])
							);
							glCopyNamedBufferSubData
							(
								environment.state.gravity_sources_buffer, environment.state.gravity_sources_buffer,
								environment.state.gravity_sources_buffer_strengths_offset + environment.state.current_gravity_source_count * environment.state.gravity_sources_buffer_strengths_stride,
								environment.state.gravity_sources_buffer_strengths_offset + hovered_gravity_source * environment.state.gravity_sources_buffer_strengths_stride,
								sizeof(GLfloat)
							);
						}
					}
					else
					{
						GLenum fence_status = glClientWaitSync(environment.state.physics_tick_results_fence, 0u, 0u);
						while (fence_status != GL_ALREADY_SIGNALED && fence_status != GL_CONDITION_SATISFIED)
						{
							// TODO: Do something useful but not necessary while we wait. 
							// Example: Optimize proximity tree.
							fence_status = glClientWaitSync(environment.state.physics_tick_results_fence, 0u, 0u);
						}

						if (util::proximity::is_empty(environment.state.proximity_tree))
						{

						}
						else if (util::proximity::has_single_node(environment.state.proximity_tree, game_logic_MAX_LEAF_COUNT(environment)))
						{

						}
						else
						{
							GLint cursor_world_x, cursor_world_y;
							window_to_world::window_screen_cursor_position_to_world_position
							(
								environment,
								&cursor_world_x, &cursor_world_y
							);

							struct On_Leaf_Found
							{
								game_environment::Environment& environment;
								GLint const cursor_world_x;
								GLint const cursor_world_y;
								GLuint& hovered_triangle;
								GLfloat& hovered_local_x;
								GLfloat& hovered_local_y;

								void operator()(GLuint leaf_index, game_state::proximity::Node const& leaf)
								{
									if (leaf_index < game_logic_TRIANGLE_LEAFS_BASE_INDEX(environment))
									{
										return;
									}
									leaf_index -= game_logic_TRIANGLE_LEAFS_BASE_INDEX(environment);

									game_state::rigid_body::Triangle const& triangle{ environment.state.GPU_buffers.rigid_bodies.triangles.values[leaf_index] };
									GLint body_position[4u];
									std::memcpy
									(
										&body_position,
										environment.state.GPU_buffers.rigid_bodies.positions.mapping + environment.state.GPU_buffers.rigid_bodies.positions.p_offset + triangle.body * environment.state.GPU_buffers.rigid_bodies.positions.p_stride,
										sizeof(body_position)
									);
									GLfloat local_x;
									GLfloat local_y;
									if (
										triangle_contains_point(environment, leaf_index, cursor_world_x, cursor_world_y, local_x, local_y) &&
										leaf_index < hovered_triangle
										)
									{
										hovered_triangle = leaf_index;
										hovered_local_x = local_x;
										hovered_local_y = local_y;
									}
								}
							};
							GLuint hovered_triangle{ game_logic__util__proximity_NULL_INDEX };
							GLfloat hovered_local_x;
							GLfloat hovered_local_y;
							On_Leaf_Found on_leaf_found{ environment, cursor_world_x, cursor_world_y, hovered_triangle, hovered_local_x, hovered_local_y };
							util::proximity::query_point_of_multinode_tree
							(
								environment.state.proximity_tree, game_logic_MAX_LEAF_COUNT(environment),
								cursor_world_x, cursor_world_y,
								on_leaf_found
							);
							if (hovered_triangle != game_logic__util__proximity_NULL_INDEX)
							{
								glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT);

								GLuint const count_jump{ environment.state.current_distance_constraint_count * environment.state.GPU_buffers.rigid_bodies.distance_constraints.distance_constraints_stride };
								glClearNamedBufferSubData
								(
									environment.state.GPU_buffers.rigid_bodies.distance_constraints.buffer,
									GL_R32UI,
									environment.state.GPU_buffers.rigid_bodies.distance_constraints.distance_constraints_bodies_offset + count_jump,
									sizeof(GLuint),
									GL_RED_INTEGER, GL_UNSIGNED_INT,
									&environment.state.GPU_buffers.rigid_bodies.triangles.values[hovered_triangle].body
								);
								// TODO: Put hovered_local_x, hovered_local_y in array from start
								GLfloat local_point[2u]
								{
									game_logic__util__spatial_TO_METERS(environment, hovered_local_x),
									game_logic__util__spatial_TO_METERS(environment, hovered_local_y)
								};
								glClearNamedBufferSubData
								(
									environment.state.GPU_buffers.rigid_bodies.distance_constraints.buffer,
									GL_RG32F,
									environment.state.GPU_buffers.rigid_bodies.distance_constraints.distance_constraints_local_points_offset + count_jump,
									sizeof(local_point),
									GL_RG, GL_FLOAT,
									local_point
								);

								/*glClearNamedBufferSubData
								(
									environment.state.distance_constraint_buffer,
									GL_R32F,
									environment.state.distance_constraint_buffer_distance_constraints_impulse_offset + count_jump,
									sizeof(GLfloat),
									GL_R, GL_FLOAT,
									nullptr
								);*/

								environment.state.distance_constraint_start_triangle = hovered_triangle;
							}
						}
					}
				}
				break;
			case GLFW_RELEASE:
				if (!environment.state.point_grabbed)
				{
					if (environment.state.distance_constraint_start_triangle != game_logic__util__proximity_NULL_INDEX)
					{
						GLenum fence_status = glClientWaitSync(environment.state.physics_tick_results_fence, 0u, 0u);
						while (fence_status != GL_ALREADY_SIGNALED && fence_status != GL_CONDITION_SATISFIED)
						{
							// TODO: Do something useful but not necessary while we wait. 
							// Example: Optimize proximity tree.
							fence_status = glClientWaitSync(environment.state.physics_tick_results_fence, 0u, 0u);
						}

						if (util::proximity::is_empty(environment.state.proximity_tree))
						{

						}
						else if (util::proximity::has_single_node(environment.state.proximity_tree, game_logic_MAX_LEAF_COUNT(environment)))
						{

						}
						else
						{
							GLint cursor_world_x, cursor_world_y;
							window_to_world::window_screen_cursor_position_to_world_position
							(
								environment,
								&cursor_world_x, &cursor_world_y
							);

							struct On_Leaf_Found
							{
								game_environment::Environment& environment;
								GLint const cursor_world_x;
								GLint const cursor_world_y;
								GLuint& hovered_triangle;
								GLfloat& hovered_local_x;
								GLfloat& hovered_local_y;

								void operator()(GLuint leaf_index, game_state::proximity::Node const& leaf)
								{
									if (leaf_index < game_logic_TRIANGLE_LEAFS_BASE_INDEX(environment))
									{
										return;
									}
									leaf_index -= game_logic_TRIANGLE_LEAFS_BASE_INDEX(environment);

									game_state::rigid_body::Triangle const& triangle{ environment.state.GPU_buffers.rigid_bodies.triangles.values[leaf_index] };
									GLint body_position[4u];
									std::memcpy
									(
										&body_position,
										environment.state.GPU_buffers.rigid_bodies.positions.mapping + environment.state.GPU_buffers.rigid_bodies.positions.p_offset + triangle.body * environment.state.GPU_buffers.rigid_bodies.positions.p_stride,
										sizeof(body_position)
									);
									GLfloat local_x;
									GLfloat local_y;
									if (
										triangle_contains_point(environment, leaf_index, cursor_world_x, cursor_world_y, local_x, local_y) &&
										leaf_index < hovered_triangle
										)
									{
										hovered_triangle = leaf_index;
										hovered_local_x = local_x;
										hovered_local_y = local_y;
									}
								}
							};
							GLuint hovered_triangle{ game_logic__util__proximity_NULL_INDEX };
							GLfloat hovered_local_x;
							GLfloat hovered_local_y;
							On_Leaf_Found on_leaf_found{ environment, cursor_world_x, cursor_world_y, hovered_triangle, hovered_local_x, hovered_local_y };
							util::proximity::query_point_of_multinode_tree
							(
								environment.state.proximity_tree, game_logic_MAX_LEAF_COUNT(environment),
								cursor_world_x, cursor_world_y,
								on_leaf_found
							);
							if (hovered_triangle != game_logic__util__proximity_NULL_INDEX && hovered_triangle != environment.state.distance_constraint_start_triangle)
							{
								glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);

								GLuint const count_jump{ environment.state.current_distance_constraint_count * environment.state.GPU_buffers.rigid_bodies.distance_constraints.distance_constraints_stride };
								glClearNamedBufferSubData
								(
									environment.state.GPU_buffers.rigid_bodies.distance_constraints.buffer,
									GL_R32UI,
									environment.state.GPU_buffers.rigid_bodies.distance_constraints.distance_constraints_bodies_offset + 4u + count_jump,
									sizeof(GLuint),
									GL_RED_INTEGER, GL_UNSIGNED_INT,
									&environment.state.GPU_buffers.rigid_bodies.triangles.values[hovered_triangle].body
								);
								// TODO: Put hovered_local_x, hovered_local_y in array from start
								GLfloat local_point[2u]
								{
									game_logic__util__spatial_TO_METERS(environment, hovered_local_x),
									game_logic__util__spatial_TO_METERS(environment, hovered_local_y)
								};
								glClearNamedBufferSubData
								(
									environment.state.GPU_buffers.rigid_bodies.distance_constraints.buffer,
									GL_RG32F,
									environment.state.GPU_buffers.rigid_bodies.distance_constraints.distance_constraints_local_points_offset + environment.state.GPU_buffers.rigid_bodies.distance_constraints.distance_constraints_local_points_stride + count_jump,
									sizeof(local_point),
									GL_RG, GL_FLOAT,
									local_point
								);

								/*GLfloat const max_distance{2.0f};
								glClearNamedBufferSubData
								(
									environment.state.distance_constraint_buffer,
									GL_R32F,
									environment.state.distance_constraint_buffer_distance_constraints_max_distance_offset + count_jump,
									sizeof(GLfloat),
									GL_R, GL_FLOAT,
									&max_distance
								);*/

								glUseProgram(environment.state.add_distance_constraint_shader);
								glDispatchCompute(1u, 1u, 1u);

								++environment.state.current_distance_constraint_count;
							}
						}
					}
				}
				environment.state.distance_constraint_start_triangle = game_logic__util__proximity_NULL_INDEX;
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
		if (!environment.state.point_grabbed)
		{
			GLint cursor_world_x, cursor_world_y;
			window_to_world::window_screen_cursor_position_to_world_position
			(
				environment,
				&cursor_world_x, &cursor_world_y
			);

			GLuint hovered_gravity_source;
			for (hovered_gravity_source = 0u; hovered_gravity_source < environment.state.current_gravity_source_count; ++hovered_gravity_source)
			{
				::util::math::Vector_2D const& gravity_source_position{ environment.state.gravity_source_positions[hovered_gravity_source] };
				GLfloat diff_x{ static_cast<GLfloat>(cursor_world_x - gravity_source_position.x) };
				GLfloat diff_y{ static_cast<GLfloat>(cursor_world_y - gravity_source_position.y) };
				GLfloat distance_squared{ diff_x * diff_x + diff_y * diff_y };
				if (distance_squared <= GRAVITY_SOURCE_GRAB_RADIUS(environment) * GRAVITY_SOURCE_GRAB_RADIUS(environment))
				{
					break;
				}
			}
			if (hovered_gravity_source != environment.state.current_gravity_source_count)
			{
				GLfloat strength = environment.state.gravity_source_strengths[hovered_gravity_source] + y_offset * 1.0f * game_logic__util__spatial_METER(environment);
				environment.state.gravity_source_strengths[hovered_gravity_source] = strength;
				glClearNamedBufferSubData
				(
					environment.state.gravity_sources_buffer,
					GL_R32F,
					environment.state.gravity_sources_buffer_strengths_offset + hovered_gravity_source * environment.state.gravity_sources_buffer_strengths_stride, 
					sizeof(GLfloat),
					GL_RED, GL_FLOAT,
					&strength
				);
				return;
			}
		}

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
		GLint cursor_world_position[2u];
		window_to_world::window_screen_cursor_position_to_world_position(environment, &cursor_world_position[0], &cursor_world_position[1]);
		glClearNamedBufferSubData
		(
			environment.state.cursor_position_buffer,
			GL_RG32I,
			environment.state.cursor_position_buffer_position_offset, sizeof(cursor_world_position),
			GL_RG_INTEGER, GL_INT,
			cursor_world_position
		);
		if (environment.state.grabbed_gravity_source != game_logic__util__proximity_NULL_INDEX)
		{
			environment.state.gravity_source_positions[environment.state.grabbed_gravity_source] = ::util::math::Vector_2D{ cursor_world_position[0u], cursor_world_position[1u] };
			glClearNamedBufferSubData
			(
				environment.state.gravity_sources_buffer, 
				GL_RG32I,
				environment.state.gravity_sources_buffer_positions_offset + environment.state.grabbed_gravity_source * environment.state.gravity_sources_buffer_positions_stride, 
				sizeof(cursor_world_position), 
				GL_RG_INTEGER, GL_INT, 
				cursor_world_position
			);
		}

		// TODO: See if we can load and run these opengl commands in GPU buffer (using indirect something)
		if (environment.state.physics_running)
		{
			tick_physics(environment);
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
		draw_inner_bounding_boxes(environment, node.children[0u]);
		draw_inner_bounding_boxes(environment, node.children[1u]);
		glUniform4i
		(
			environment.state.parent_bounding_box_uniform_location,
			node.bounding_box.min.x, node.bounding_box.min.y, 
			node.bounding_box.max.x, node.bounding_box.max.y
		);
		glDrawArrays(GL_LINES, 0, 8);
	}

	bool triangle_contains_point
	(
		game_environment::Environment const& environment, GLuint const triangle_index, 
		GLint x, GLint y
	)
	{
		game_state::rigid_body::Triangle const& triangle{ environment.state.GPU_buffers.rigid_bodies.triangles.values[triangle_index] };
		util::rigid_body::Position body_position;
		std::memcpy
		(
			&body_position,
			environment.state.GPU_buffers.rigid_bodies.positions.mapping + environment.state.GPU_buffers.rigid_bodies.positions.p_offset + triangle.body * environment.state.GPU_buffers.rigid_bodies.positions.p_stride,
			sizeof(body_position)
		);
		GLfloat local_x = static_cast<GLfloat>(x - body_position.position.x);
		GLfloat local_y = static_cast<GLfloat>(y - body_position.position.y);

		// TODO: Do not loose precision for large angles
		GLfloat angle = game_logic__util__spatial_TO_RADIANS(environment, body_position.angle);
		GLfloat right_x = std::cos(angle);
		GLfloat right_y = std::sin(angle);

		GLfloat const old_local_x{ local_x };
		local_x = local_x * right_x + local_y * right_y;
		local_y = local_y * right_x - old_local_x * right_y;

		GLfloat vertex_0_x{ environment.state.GPU_buffers.rigid_bodies.triangles.vertices.values[triangle.vertices[0u]][0u] };
		GLfloat vertex_0_y{ environment.state.GPU_buffers.rigid_bodies.triangles.vertices.values[triangle.vertices[0u]][1u] };

		GLfloat vertex_1_x{ environment.state.GPU_buffers.rigid_bodies.triangles.vertices.values[triangle.vertices[1u]][0u] };
		GLfloat vertex_1_y{ environment.state.GPU_buffers.rigid_bodies.triangles.vertices.values[triangle.vertices[1u]][1u] };

		GLfloat vertex_2_x{ environment.state.GPU_buffers.rigid_bodies.triangles.vertices.values[triangle.vertices[2u]][0u] };
		GLfloat vertex_2_y{ environment.state.GPU_buffers.rigid_bodies.triangles.vertices.values[triangle.vertices[2u]][1u] };

		GLfloat normal_0_x = vertex_1_y - vertex_0_y;
		GLfloat normal_0_y = vertex_0_x - vertex_1_x;

		GLfloat normal_1_x = vertex_2_y - vertex_1_y;
		GLfloat normal_1_y = vertex_1_x - vertex_2_x;

		GLfloat normal_2_x = vertex_0_y - vertex_2_y;
		GLfloat normal_2_y = vertex_2_x - vertex_0_x;

		return
		(
			local_x * normal_0_x + local_y * normal_0_y <= vertex_0_x * normal_0_x + vertex_0_y * normal_0_y &&
			local_x * normal_1_x + local_y * normal_1_y <= vertex_1_x * normal_1_x + vertex_1_y * normal_1_y &&
			local_x * normal_2_x + local_y * normal_2_y <= vertex_2_x * normal_2_x + vertex_2_y * normal_2_y
		);
	}

	// TODO: Rename to draw to not confuse with arbitrary OpenGL rendering commands 
	// which includes compute shaders
	void render(game_environment::Environment& environment)
	{
		// TODO: Indirect drawing for increased performance

		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

		update_GPU_camera(environment);

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, environment.state.fluid_framebuffer);

		GLfloat const fluid_clear_color[4u]{ 0.0f, 0.0f, 0.0f, 0.0f }; // IMPORTANT!!!! ALPHA SHOULD BE 0
		glClearBufferfv(GL_COLOR, 1, fluid_clear_color);

		glUseProgram(environment.state.fluid_particles_draw_shader);
		glDrawArrays(GL_TRIANGLES, 0, environment.state.current_fluid_particle_count * 6u);

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0u);
		//glClear(GL_COLOR_BUFFER_BIT);	// IMPORTANT TODO: REMOVE!!!

		glUseProgram(environment.state.fluid_draw_shader);
		glDrawArrays(GL_TRIANGLES, 0, 6u);

		if (environment.state.debug_fluid_particles_visible)
		{
			glUseProgram(environment.state.debug_fluid_particles_draw_shader);
			glPointSize(5.0f);
			glDrawArrays(GL_POINTS, 0, environment.state.current_fluid_particle_count);
		}

		glUseProgram(environment.state.triangle_draw_shader);
		glDrawArrays(GL_TRIANGLES, 0, environment.state.GPU_buffers.rigid_bodies.triangles.current_count * 3u);

		if (environment.state.triangle_wireframes_visible)
		{
			glUseProgram(environment.state.triangle_wireframes_draw_shader);
			glDrawArrays(GL_LINES, 0, environment.state.GPU_buffers.rigid_bodies.triangles.current_count * 6u);
		}

		if (environment.state.rigid_bodies_visible)
		{
			glUseProgram(environment.state.rigid_body_debug_rendering_shader);
			glDrawArrays(GL_LINES, 0, environment.state.current_rigid_body_count * 4u);
		}

		if (environment.state.triangle_normals_visible)
		{
			glUseProgram(environment.state.triangle_normal_draw_shader);
			glDrawArrays(GL_LINES, 0, environment.state.GPU_buffers.rigid_bodies.triangles.current_count * 6u);
		}

		if (environment.state.leaf_bounding_boxes_visible)
		{
			glUseProgram(environment.state.triangle_bounding_box_draw_shader);
			glDrawArrays(GL_LINES, 0, environment.state.GPU_buffers.rigid_bodies.triangles.current_count * 8u);
			
			glUseProgram(environment.state.fluid_bounding_boxes_draw_shader);
			glDrawArrays(GL_LINES, 0, environment.state.current_fluid_particle_count * 8u);
		}

		if (environment.state.parent_bounding_boxes_visible && !util::proximity::is_empty(environment.state.proximity_tree))
		{
			glUseProgram(environment.state.parent_bounding_box_draw_shader);
			draw_inner_bounding_boxes(environment, environment.state.proximity_tree.root);
		}

		if (environment.state.leaf_contacts_visible)
		{
			glUseProgram(environment.state.fluid_leaf_contacts_draw_shader);
			glDrawArrays(GL_LINES, 0, environment.state.GPU_buffers.fluid.contact_count.current_contact_count * 2u);

			glUseProgram(environment.state.fluid_triangle_leaf_contacts_draw_shader);
			glDrawArrays(GL_LINES, 0, environment.state.GPU_buffers.fluid_triangle.contact_count.current_contact_count * 2u);

			glUseProgram(environment.state.leaf_triangle_contact_draw_shader);
			glDrawArrays(GL_LINES, 0, environment.state.current_triangle_contact_count * 2u);
		}

		if (environment.state.contact_point_positions_visible)
		{
			glPointSize(10.0f);

			glUseProgram(environment.state.fluid_triangle_contact_points_draw_shader);
			glDrawArrays(GL_POINTS, 0, environment.state.GPU_buffers.fluid_triangle.contact_count.current_contact_count);

			glUseProgram(environment.state.contact_point_positions_draw_shader);
			glDrawArrays(GL_POINTS, 0, environment.state.current_triangle_contact_count * 4u);
		}

		//glUseProgram(environment.state.contact_point_offsets_draw_shader);
		//glDrawArrays(GL_LINES, 0, environment.state.current_contact_count * 8u);


		if (environment.state.contact_basis_visible)
		{
			glUseProgram(environment.state.fluid_triangle_contact_basis_draw_shader);
			glDrawArrays(GL_LINES, 0, environment.state.GPU_buffers.fluid_triangle.contact_count.current_contact_count * 2u);

			glUseProgram(environment.state.contact_basis_draw_shader);
			glDrawArrays(GL_LINES, 0, environment.state.current_triangle_contact_count * 4u);
		}

		if (environment.state.contact_impulses_visible)
		{
			glUseProgram(environment.state.contact_impulses_draw_shader);
			glDrawArrays(GL_LINES, 0, environment.state.current_triangle_contact_count * 16u);
		}

		GLuint hovered_gravity_source = environment.state.current_gravity_source_count;

		if (!environment.state.point_grabbed)
		{
			if (environment.state.grabbed_triangle != game_logic__util__proximity_NULL_INDEX)
			{
				glUseProgram(environment.state.hovered_triangle_wireframe_draw_shader);
				glUniform1ui(environment.state.hovered_triangle_wireframe_hovered_triangle_uniform_location, environment.state.grabbed_triangle);
				glDrawArrays(GL_LINES, 0, 6u);
			}
			else
			{
				GLFWcursor* cursor = nullptr;

				GLint cursor_world_x, cursor_world_y;
				window_to_world::window_screen_cursor_position_to_world_position
				(
					environment,
					&cursor_world_x, &cursor_world_y
				);

				for (hovered_gravity_source = 0u; hovered_gravity_source < environment.state.current_gravity_source_count; ++hovered_gravity_source)
				{
					::util::math::Vector_2D const& gravity_source_position{ environment.state.gravity_source_positions[hovered_gravity_source] };
					GLfloat diff_x{ static_cast<GLfloat>(cursor_world_x - gravity_source_position.x) };
					GLfloat diff_y{ static_cast<GLfloat>(cursor_world_y - gravity_source_position.y) };
					GLfloat distance_squared{ diff_x * diff_x + diff_y * diff_y };
					if (distance_squared <= GRAVITY_SOURCE_GRAB_RADIUS(environment) * GRAVITY_SOURCE_GRAB_RADIUS(environment))
					{
						cursor = environment.state.cursor_types.move_cursor;
						break;
					}
				}
				if (hovered_gravity_source == environment.state.current_gravity_source_count)
				{
					GLenum fence_status = glClientWaitSync(environment.state.physics_tick_results_fence, 0u, 0u);
					while (fence_status != GL_ALREADY_SIGNALED && fence_status != GL_CONDITION_SATISFIED)
					{
						// TODO: Do something useful but not necessary while we wait. 
						// Example: Optimize proximity tree.
						fence_status = glClientWaitSync(environment.state.physics_tick_results_fence, 0u, 0u);
					}

					if (util::proximity::is_empty(environment.state.proximity_tree))
					{

					}
					else if (util::proximity::has_single_node(environment.state.proximity_tree, game_logic_MAX_LEAF_COUNT(environment)))
					{

					}
					else
					{
						struct On_Leaf_Found
						{
							game_environment::Environment& environment;
							GLint const cursor_world_x;
							GLint const cursor_world_y;
							GLuint& hovered_triangle;

							void operator()(GLuint leaf_index, game_state::proximity::Node const& leaf)
							{
								if (leaf_index < game_logic_TRIANGLE_LEAFS_BASE_INDEX(environment))
								{
									return;
								}
								leaf_index -= game_logic_TRIANGLE_LEAFS_BASE_INDEX(environment);

								game_state::rigid_body::Triangle const& triangle{ environment.state.GPU_buffers.rigid_bodies.triangles.values[leaf_index] };
								GLint body_position[4u];
								std::memcpy
								(
									&body_position,
									environment.state.GPU_buffers.rigid_bodies.positions.mapping + environment.state.GPU_buffers.rigid_bodies.positions.p_offset + triangle.body * environment.state.GPU_buffers.rigid_bodies.positions.p_stride,
									sizeof(body_position)
								);
								if (
									triangle_contains_point(environment, leaf_index, cursor_world_x, cursor_world_y) &&
									leaf_index < hovered_triangle
									)
								{
									hovered_triangle = leaf_index;
								}
							}
						};
						GLuint hovered_triangle{ game_logic__util__proximity_NULL_INDEX };
						On_Leaf_Found on_leaf_found{ environment, cursor_world_x, cursor_world_y, hovered_triangle };
						util::proximity::query_point_of_multinode_tree
						(
							environment.state.proximity_tree, game_logic_MAX_LEAF_COUNT(environment),
							cursor_world_x, cursor_world_y,
							on_leaf_found
						);
						if (hovered_triangle != game_logic__util__proximity_NULL_INDEX)
						{
							glUseProgram(environment.state.hovered_triangle_wireframe_draw_shader);
							glUniform1ui(environment.state.hovered_triangle_wireframe_hovered_triangle_uniform_location, hovered_triangle);
							glDrawArrays(GL_LINES, 0, 6u);

							cursor = environment.state.cursor_types.point_cursor;
						}
					}
				}
				glfwSetCursor(environment.window, cursor);
			}
		}

		if 
		(
			environment.state.gravity_visible || 
			hovered_gravity_source != environment.state.current_gravity_source_count || 
			environment.state.grabbed_gravity_source != game_logic__util__proximity_NULL_INDEX
		)
		{
			GLfloat const x_step{ GRAVITY_SAMPLE_STEP(environment) * game_logic__util__projection_SCALE_X(environment) };
			GLfloat const y_step{ GRAVITY_SAMPLE_STEP(environment) * game_logic__util__projection_SCALE_Y(environment) };
			GLuint const width{ 1u + 2u * static_cast<GLuint>(1.0f / x_step) };
			GLuint const height{ 1u + 2u * static_cast<GLuint>(1.0f / y_step) };
			GLuint const grid_point_count{ width * height };

			glUseProgram(environment.state.gravity_grid_points_draw_shader);
			glPointSize(5.0f);
			glDrawArrays(GL_POINTS, 0, grid_point_count);

			glUseProgram(environment.state.gravity_directions_draw_shader);
			glDrawArrays(GL_LINES, 0, grid_point_count * 2u);
		}

		glEnablei(GL_BLEND, 0u);
		glUseProgram(environment.state.gravity_sources_draw_shader);
		glDrawArrays(GL_TRIANGLES, 0, environment.state.current_gravity_source_count * 6u);
		glDisablei(GL_BLEND, 0u);

		if (hovered_gravity_source != environment.state.current_gravity_source_count)
		{
			glUseProgram(environment.state.hovered_gravity_source_wireframe_draw_shader);
			glUniform1ui(environment.state.hovered_gravity_source_wireframe_draw_shader_hovered_gravity_source_uniform_location, hovered_gravity_source);
			glDrawArrays(GL_TRIANGLES, 0, 6u);
		}

		glUseProgram(environment.state.distance_constraints_draw_shader);
		glDrawArrays(GL_LINES, 0, environment.state.current_distance_constraint_count * 2u);

		if (environment.state.distance_constraint_start_triangle != game_logic__util__proximity_NULL_INDEX)
		{
			glUseProgram(environment.state.distance_constraint_start_draw_shader);
			glDrawArrays(GL_LINES, 0, 2u);
		}

		if (environment.state.grabbed_triangle != game_logic__util__proximity_NULL_INDEX)
		{
			glUseProgram(environment.state.cursor_constraint_draw_shader);
			glDrawArrays(GL_LINES, 0, 2u);
		}

		glUseProgram(environment.state.cursor_position_draw_shader);
		glPointSize(5.0f);
		glDrawArrays(GL_POINTS, 0, 1u);
	}

	void free(game_environment::Environment& environment)
	{
		delete_fluid_texture_and_framebuffer(environment);

		glUnmapNamedBuffer(environment.state.GPU_buffers.rigid_bodies.positions.buffer);
		glUnmapNamedBuffer(environment.state.GPU_buffers.rigid_bodies.triangles.changed_bounding_boxes.buffer);
		glUnmapNamedBuffer(environment.state.contact_buffer);

		::util::shader::delete_program(environment.state.shader);

		delete[] environment.state.camera_send_buffer;
		// TODO: Probably flip position and velocity buffers
		GLuint buffers[]
		{ 
			environment.state.camera_buffer, 
			environment.state.GPU_buffers.rigid_bodies.positions.buffer,
			environment.state.GPU_buffers.rigid_bodies.velocities.buffer,
			environment.state.GPU_buffers.rigid_bodies.triangles.buffer,
			environment.state.GPU_buffers.rigid_bodies.triangles.vertices.buffer,
			environment.state.GPU_buffers.rigid_bodies.triangles.bounding_boxes.buffer, 
			environment.state.GPU_buffers.rigid_bodies.triangles.changed_bounding_boxes.buffer, 
			environment.state.contact_buffer, 
			environment.state.contact_surface_buffer, 
			environment.state.GPU_buffers.rigid_bodies.triangles.contact_count.buffer, 
			environment.state.GPU_buffers.rigid_bodies.triangles.persistent_contact_count.buffer, 
			environment.state.GPU_buffers.rigid_bodies.velocities.snapshot_buffer, 
			environment.state.GPU_buffers.rigid_bodies.positions.snapshot_buffer,
			environment.state.cursor_position_buffer, 
			environment.state.cursor_constrained_point_buffer, 
			environment.state.cursor_constraint_buffer, 
			environment.state.GPU_buffers.rigid_bodies.distance_constraints.buffer,
			environment.state.GPU_buffers.fluid.positions.buffer,
			environment.state.GPU_buffers.fluid.velocities.buffer,
			environment.state.GPU_buffers.fluid.bounding_boxes.buffer,
			environment.state.GPU_buffers.fluid.changed_bounding_boxes.buffer, 
			environment.state.GPU_buffers.fluid.contacts.buffer, 
			environment.state.GPU_buffers.fluid.contact_count.buffer,
			environment.state.GPU_buffers.fluid.velocities.snapshot_buffer,
			environment.state.GPU_buffers.fluid_triangle.contacts.buffer,
			environment.state.GPU_buffers.fluid_triangle.contact_count.buffer,
			environment.state.gravity_sources_buffer, 
			environment.state.count_buffer, 
			environment.state.body_masses_buffer
		};
		glDeleteBuffers(std::size(buffers), buffers);

		glDeleteVertexArrays(1, &environment.state.vao);
		glDeleteBuffers(1, &environment.state.vbo);

		cursor_types::free(environment);

		// TODO: Delete shader programs???
	}
}
