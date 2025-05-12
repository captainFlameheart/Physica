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
#include "game_logic/util/proximity/update_contacts.h"
#include "game_logic/util/proximity/insert_leaf_to_empty_tree.h"
#include "game_logic/util/proximity/insert_leaf_to_nonempty_tree.h"
#include "game_logic/util/proximity/print_tree.h"
#include "game_logic/util/proximity/print_leaf_contacts.h"
#include "game_logic/util/proximity/is_empty.h"
#include "game_logic/util/proximity/has_single_node.h"
#include "game_logic/util/proximity/print_bounding_box.h"
#include "game_logic/util/proximity/compute_height.h"
#include "game_logic/util/proximity/update_contacts.h"
#include "game_logic/util/proximity/query_point_of_multinode_tree.h"
#include <algorithm>

#define game_logic_MAX_RIGID_BODY_COUNT(environment) \
	20u * game_logic__util__rigid_body_VELOCITY_INTEGRATION_LOCAL_SIZE(environment)

#define game_logic_MAX_TRIANGLE_COUNT(environment) \
	20u * game_logic__util__rigid_body_TRIANGLE_BOUNDING_BOX_UPDATE_LOCAL_SIZE(environment)

#define game_logic_MAX_VERTEX_COUNT(environment) \
	3u * game_logic_MAX_TRIANGLE_COUNT(environment)

#define game_logic_MAX_LEAF_COUNT(environment) \
	game_logic_MAX_TRIANGLE_COUNT(environment)

#define game_logic_MAX_CONTACT_COUNT(environment) \
	2000u * game_logic_MAX_TRIANGLE_COUNT(environment)

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
	0.05f//game_logic__util__spatial_FLOAT_FROM_METERS(environment, 0.05f)

#define game_logic_DISTANCE_CONSTRAINT_IMPULSE_SCALE(environment) 0.05f

#define game_logic_CURSOR_CONSTRAINT_VELOCITY_SCALE(environment) 0.1f
#define game_logic_CURSOR_CONSTRAINT_IMPULSE_SCALE(environment) 0.05f
#define game_logic_CURSOR_CONSTRAINT_MAX_IMPULSE(environment) 0.5f

// TODO: Store separate masses for each body in buffer
#define INVERSE_MASS 1.0f
#define INVERSE_INERTIA 2.0f

// TODO: Make this depend on triangle materials
#define FRICTION_COEFFICIENT 0.5f

// To reduce compile times (due to driver bug), 
// should be disabled for release/performance testing
#define USE_DYNAMIC_SIZES true

// TODO: USE EXCLUSIVE OR IN PROXIMITY UTIL WHEN FLIPPING A BOOLEAN UNSIGNED INT

namespace game_logic
{
	void initialize(game_environment::Environment& environment)
	{
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
#if USE_DYNAMIC_SIZES == true
		max_rigid_body_count_definition = util_shader_DEFINE("MAX_RIGID_BODY_COUNT", "");
		max_triangle_count_definition = util_shader_DEFINE("MAX_TRIANGLE_COUNT", "");
		max_vertex_count_definition = util_shader_DEFINE("MAX_VERTEX_COUNT", "");
		max_contact_count_definition = util_shader_DEFINE("MAX_CONTACT_COUNT", "");
		max_distance_constraint_count_definition = util_shader_DEFINE("MAX_DISTANCE_CONSTRAINT_COUNT", "");
#else
		max_rigid_body_count_definition = util_shader_DEFINE("MAX_RIGID_BODY_COUNT", STRINGIFY(game_logic_MAX_RIGID_BODY_COUNT(environment)));
		max_triangle_count_definition = util_shader_DEFINE("MAX_TRIANGLE_COUNT", STRINGIFY(game_logic_MAX_TRIANGLE_COUNT(environment)));
		max_vertex_count_definition = util_shader_DEFINE("MAX_VERTEX_COUNT", STRINGIFY(game_logic_MAX_VERTEX_COUNT(environment)));
		max_contact_count_definition = util_shader_DEFINE("MAX_CONTACT_COUNT", STRINGIFY(game_logic_MAX_CONTACT_COUNT(environment)));
		max_distance_constraint_count_definition = util_shader_DEFINE("MAX_DISTANCE_CONSTRAINT_COUNT", STRINGIFY(game_logic_MAX_DISTANCE_CONSTRAINT_COUNT(environment)));
#endif

		std::cout << "Initializing..." << std::endl;
		std::cout << "Max contact count: " << game_logic_MAX_CONTACT_COUNT(environment) << std::endl;

		environment.state.tick = 0u;
		environment.state.physics_running = true;

		environment.state.grabbed_triangle = game_logic__util__proximity_NULL_INDEX;
		environment.state.distance_constraint_start_triangle = game_logic__util__proximity_NULL_INDEX;

		environment.state.grab_cursor = glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);
		environment.state.point_cursor = glfwCreateStandardCursor(GLFW_HAND_CURSOR);

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
		std::cout << "Dummy shader compiled" << std::endl;

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
		// TODO: REMOVE
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//glEnable(GL_BLEND);
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
			util_shader_DEFINE("COLOR", "vec4(0.3, 0.3, 0.3, 1.0)"),
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
		environment.state.leaf_contact_draw_shader = ::util::shader::create_program(vertex_shader, fragment_shader);
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

		::util::shader::set_shader_statically
		(
			compute_shader,
			util_shader_VERSION, 
			max_rigid_body_count_definition,
			util_shader_DEFINE("POSITION_BINDING", STRINGIFY(game_logic__util_RIGID_BODY_POSITION_BINDING)),
			util_shader_DEFINE("VELOCITY_BINDING", STRINGIFY(game_logic__util_RIGID_BODY_VELOCITY_BINDING)),
			util_shader_DEFINE("LOCAL_SIZE", STRINGIFY(game_logic__util__rigid_body_VELOCITY_INTEGRATION_LOCAL_SIZE(environment))),
			::util::shader::file_to_string("util/rigid_body_velocity_integration.comp")
		);
		environment.state.rigid_body_velocity_integration_shader = ::util::shader::create_program(compute_shader);
		std::cout << "Rigid body velocity integration shader compiled" << std::endl;

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
			util_shader_DEFINE("INVERSE_MASS", STRINGIFY(INVERSE_MASS)),
			util_shader_DEFINE("INVERSE_INERTIA", STRINGIFY(INVERSE_INERTIA)),
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
			util_shader_DEFINE("INVERSE_MASS", STRINGIFY(INVERSE_MASS)),
			util_shader_DEFINE("INVERSE_INERTIA", STRINGIFY(INVERSE_INERTIA)),
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
			util_shader_DEFINE("INVERSE_MASS", STRINGIFY(INVERSE_MASS)), 
			util_shader_DEFINE("INVERSE_INERTIA", STRINGIFY(INVERSE_INERTIA)),
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
			util_shader_DEFINE("INVERSE_MASS", STRINGIFY(INVERSE_MASS)),
			util_shader_DEFINE("INVERSE_INERTIA", STRINGIFY(INVERSE_INERTIA)),
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
			util_shader_DEFINE("INVERSE_MASS", STRINGIFY(INVERSE_MASS)),
			util_shader_DEFINE("INVERSE_INERTIA", STRINGIFY(INVERSE_INERTIA)),
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
			util_shader_DEFINE("INVERSE_MASS", STRINGIFY(INVERSE_MASS)),
			util_shader_DEFINE("INVERSE_INERTIA", STRINGIFY(INVERSE_INERTIA)),
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
			util_shader_DEFINE("INVERSE_MASS", STRINGIFY(INVERSE_MASS)),
			util_shader_DEFINE("INVERSE_INERTIA", STRINGIFY(INVERSE_INERTIA)),
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
			util_shader_DEFINE("INVERSE_MASS", STRINGIFY(INVERSE_MASS)),
			util_shader_DEFINE("INVERSE_INERTIA", STRINGIFY(INVERSE_INERTIA)),
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
			util_shader_DEFINE("INVERSE_MASS", STRINGIFY(INVERSE_MASS)),
			util_shader_DEFINE("INVERSE_METER", STRINGIFY(game_logic__util__spatial_METER_INVERSE(environment))),
			util_shader_DEFINE("INVERSE_RADIAN", STRINGIFY(game_logic__util__spatial_RADIAN_INVERSE(environment))),
			util_shader_DEFINE("INVERSE_INERTIA", STRINGIFY(INVERSE_INERTIA)),
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
			util_shader_DEFINE("INVERSE_MASS", STRINGIFY(INVERSE_MASS)),
			util_shader_DEFINE("INVERSE_INERTIA", STRINGIFY(INVERSE_INERTIA)),
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
			environment.state.rigid_body_position_buffer, 
			environment.state.rigid_body_velocity_buffer, 
			environment.state.triangle_buffer, 
			environment.state.vertex_buffer, 
			environment.state.bounding_box_buffer, 
			environment.state.changed_bounding_box_buffer, 
			environment.state.contact_buffer,
			environment.state.contact_surface_buffer, 
			environment.state.contact_count_buffer, 
			environment.state.persistent_contact_count_buffer, 
			environment.state.rigid_body_velocity_snapshot_buffer, 
			environment.state.rigid_body_position_snapshot_buffer, 
			environment.state.cursor_position_buffer, 
			environment.state.cursor_constrained_point_buffer, 
			environment.state.cursor_constraint_buffer, 
			environment.state.distance_constraint_buffer
		};
		glCreateBuffers(std::size(buffers), buffers);
		environment.state.camera_buffer = buffers[0u];
		environment.state.rigid_body_position_buffer = buffers[1u];
		environment.state.rigid_body_velocity_buffer = buffers[2u];
		environment.state.triangle_buffer = buffers[3u];
		environment.state.vertex_buffer = buffers[4u];
		environment.state.bounding_box_buffer = buffers[5u];
		environment.state.changed_bounding_box_buffer = buffers[6u];
		environment.state.contact_buffer = buffers[7u];
		environment.state.contact_surface_buffer = buffers[8u];
		environment.state.contact_count_buffer = buffers[9u];
		environment.state.persistent_contact_count_buffer = buffers[10u];
		environment.state.rigid_body_velocity_snapshot_buffer = buffers[11u];
		environment.state.rigid_body_position_snapshot_buffer = buffers[12u];
		environment.state.cursor_position_buffer = buffers[13u];
		environment.state.cursor_constrained_point_buffer = buffers[14u];
		environment.state.cursor_constraint_buffer = buffers[15u];
		environment.state.distance_constraint_buffer = buffers[16u];

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

		environment.state.current_rigid_body_count = 10u * game_logic__util__rigid_body_TRIANGLE_BOUNDING_BOX_UPDATE_LOCAL_SIZE(environment);//500000u;
		environment.state.current_triangle_count = 1u * environment.state.current_rigid_body_count;
		environment.state.current_contact_count = 0u;
		environment.state.current_persistent_contact_count = 0u;
		environment.state.current_distance_constraint_count = 0u;

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

#if USE_DYNAMIC_SIZES == true
			environment.state.rigid_body_position_buffer_size = environment.state.rigid_body_position_buffer_p_offset + game_logic_MAX_RIGID_BODY_COUNT(environment) * environment.state.rigid_body_position_buffer_p_stride;
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
				GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT //0u
			);

			delete[] initial_positions;

			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, game_logic__util_RIGID_BODY_POSITION_BINDING, environment.state.rigid_body_position_buffer);

			environment.state.position_mapping = static_cast<unsigned char*>
			(
				glMapNamedBufferRange
				(
					environment.state.rigid_body_position_buffer,
					0u, environment.state.rigid_body_position_buffer_size,
					GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_FLUSH_EXPLICIT_BIT
				)
			);
		}

		{ // Position snapshot buffer
			glNamedBufferStorage
			(
				environment.state.rigid_body_position_snapshot_buffer, environment.state.rigid_body_position_buffer_size, nullptr,
				0u
			);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, game_logic__util_POSITION_SNAPSHOT_BINDING, environment.state.rigid_body_position_snapshot_buffer);
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

#if USE_DYNAMIC_SIZES == true
			environment.state.rigid_body_velocity_buffer_size = environment.state.rigid_body_velocity_buffer_v_offset + game_logic_MAX_RIGID_BODY_COUNT(environment) * environment.state.rigid_body_velocity_buffer_v_stride;
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
			unsigned char* const initial_velocities = new unsigned char[environment.state.rigid_body_velocity_buffer_size];

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

		{ // Velocity snapshot buffer
			glNamedBufferStorage
			(
				environment.state.rigid_body_velocity_snapshot_buffer, environment.state.rigid_body_velocity_buffer_size, nullptr, 
				0u
			);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, game_logic__util_VELOCITY_SNAPSHOT_BINDING, environment.state.rigid_body_velocity_snapshot_buffer);
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

#if USE_DYNAMIC_SIZES == true
			environment.state.triangle_buffer_size = environment.state.triangle_buffer_triangles_offset + game_logic_MAX_TRIANGLE_COUNT(environment) * environment.state.triangle_buffer_triangles_stride;
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
			unsigned char* const initial_triangles = new unsigned char[environment.state.triangle_buffer_size];
			environment.state.triangles = new game_state::rigid_body::Triangle[game_logic_MAX_TRIANGLE_COUNT(environment)];

			for (GLuint i = 0; i < environment.state.current_triangle_count; ++i)
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
					initial_triangles + environment.state.triangle_buffer_triangles_offset + i * environment.state.triangle_buffer_triangles_stride,
					&triangle, sizeof(triangle)
				);

				environment.state.triangles[i] = triangle;
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

#if USE_DYNAMIC_SIZES == true
			environment.state.vertex_buffer_size = environment.state.vertex_buffer_vertices_offset + game_logic_MAX_VERTEX_COUNT(environment) * environment.state.vertex_buffer_vertices_stride;
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
			unsigned char* const initial_vertices = new unsigned char[environment.state.vertex_buffer_size];
			environment.state.vertices = new GLfloat[game_logic_MAX_VERTEX_COUNT(environment)][2u];
			
			GLfloat vertex[2];
			GLfloat const r{ 0.5f };
			vertex[0] = game_logic__util__spatial_FLOAT_FROM_METERS(environment, r);
			vertex[1] = game_logic__util__spatial_FLOAT_FROM_METERS(environment, r);
			std::memcpy
			(
				initial_vertices + environment.state.vertex_buffer_vertices_offset + 0 * environment.state.vertex_buffer_vertices_stride,
				&vertex, sizeof(vertex)
			);
			environment.state.vertices[0u][0u] = vertex[0u];
			environment.state.vertices[0u][1u] = vertex[1u];

			vertex[0] = game_logic__util__spatial_FLOAT_FROM_METERS(environment, -r);
			vertex[1] = game_logic__util__spatial_FLOAT_FROM_METERS(environment, r);
			std::memcpy
			(
				initial_vertices + environment.state.vertex_buffer_vertices_offset + 1 * environment.state.vertex_buffer_vertices_stride,
				&vertex, sizeof(vertex)
			);
			environment.state.vertices[1u][0u] = vertex[0u];
			environment.state.vertices[1u][1u] = vertex[1u];

			vertex[0] = game_logic__util__spatial_FLOAT_FROM_METERS(environment, -r);
			vertex[1] = game_logic__util__spatial_FLOAT_FROM_METERS(environment, -r);
			std::memcpy
			(
				initial_vertices + environment.state.vertex_buffer_vertices_offset + 2 * environment.state.vertex_buffer_vertices_stride,
				&vertex, sizeof(vertex)
			);
			environment.state.vertices[2u][0u] = vertex[0u];
			environment.state.vertices[2u][1u] = vertex[1u];

			vertex[0] = game_logic__util__spatial_FLOAT_FROM_METERS(environment, r);
			vertex[1] = game_logic__util__spatial_FLOAT_FROM_METERS(environment, -r);
			std::memcpy
			(
				initial_vertices + environment.state.vertex_buffer_vertices_offset + 3 * environment.state.vertex_buffer_vertices_stride,
				&vertex, sizeof(vertex)
			);
			environment.state.vertices[3u][0u] = vertex[0u];
			environment.state.vertices[3u][1u] = vertex[1u];
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

#if USE_DYNAMIC_SIZES == true
			environment.state.bounding_box_buffer_size = environment.state.bounding_box_buffer_boxes_offset + game_logic_MAX_TRIANGLE_COUNT(environment) * environment.state.bounding_box_buffer_boxes_stride;
#else
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
#endif

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

#if USE_DYNAMIC_SIZES == true
			 GLint const offsets[]
			 {
				environment.state.changed_bounding_box_buffer_boxes_index_offset,
				environment.state.changed_bounding_box_buffer_boxes_min_x_offset,
				environment.state.changed_bounding_box_buffer_boxes_min_y_offset,
				environment.state.changed_bounding_box_buffer_boxes_max_x_offset,
				environment.state.changed_bounding_box_buffer_boxes_max_y_offset
			 };
			 GLint const offset{ *std::min_element(std::begin(offsets), std::end(offsets)) };
			 environment.state.changed_bounding_box_buffer_size = offset + game_logic_MAX_TRIANGLE_COUNT(environment) * environment.state.changed_bounding_box_buffer_boxes_stride;
#else
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
#endif

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
				 GL_RED_INTEGER, GL_UNSIGNED_INT, 
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

		 { // Contact buffer
			 GLuint const contacts_index
			 {
				 glGetProgramResourceIndex(environment.state.leaf_contact_draw_shader, GL_BUFFER_VARIABLE, "Contacts.contacts")
			 };

			 GLenum const prop_labels[]{ GL_OFFSET, GL_ARRAY_STRIDE };
			 GLint props[std::size(prop_labels)];
			 glGetProgramResourceiv
			 (
				 environment.state.leaf_contact_draw_shader, GL_BUFFER_VARIABLE, contacts_index,
				 std::size(prop_labels), prop_labels, 2, nullptr, props
			 );
			 // TODO: Consider putting offset and stride contigously in game state
			 environment.state.contact_buffer_contacts_offset = props[0];
			 environment.state.contact_buffer_contacts_stride = props[1];

#if USE_DYNAMIC_SIZES == true
			 environment.state.contact_buffer_size = environment.state.contact_buffer_contacts_offset + game_logic_MAX_CONTACT_COUNT(environment) * environment.state.contact_buffer_contacts_stride;
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

			 for (GLuint i{ 0u }; i < environment.state.current_contact_count; ++i)
			 {
				 GLuint contact[2]{ i, i + 1u };
				 std::memcpy
				 (
					 initial_contacts + environment.state.contact_buffer_contacts_offset + i * environment.state.contact_buffer_contacts_stride,
					 contact, sizeof(contact)
				 );
			 }

			 glNamedBufferStorage
			 (
				 environment.state.contact_buffer, environment.state.contact_buffer_size, initial_contacts,
				 GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT
			 );

			 delete[] initial_contacts;

			 glBindBufferBase(GL_SHADER_STORAGE_BUFFER, game_logic__util_CONTACT_BINDING, environment.state.contact_buffer);

			 environment.state.contact_mapping = static_cast<unsigned char*>
			(
				glMapNamedBufferRange
				(
					environment.state.contact_buffer, 
					0u, environment.state.contact_buffer_size, 
					GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_FLUSH_EXPLICIT_BIT
				)
			);
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
				environment.state.contact_surface_buffer_contact_surfaces_bodies_offset = props[0];
				environment.state.contact_surface_buffer_contact_surfaces_stride = props[1];
				environment.state.contact_surface_buffer_contact_surfaces_bodies_stride = props[2];

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
				environment.state.contact_surface_buffer_contact_surfaces_contact_point_position_0_offsets_offset = array_props[0];
				environment.state.contact_surface_buffer_contact_surfaces_contact_point_position_0_offsets_stride = array_props[1];

				GLuint const contact_surfaces_contact_point_position_1_offsets_index
				{
					glGetProgramResourceIndex(environment.state.old_triangle_contact_update_shader, GL_BUFFER_VARIABLE, "Contact_Surfaces.contact_surfaces[0].contact_point_positions[1].offsets[0]")
				};
				glGetProgramResourceiv
				(
					environment.state.old_triangle_contact_update_shader, GL_BUFFER_VARIABLE, contact_surfaces_contact_point_position_1_offsets_index,
					std::size(array_prop_labels), array_prop_labels, 2u, nullptr, array_props
				);
				environment.state.contact_surface_buffer_contact_surfaces_contact_point_position_1_offsets_offset = array_props[0];
				environment.state.contact_surface_buffer_contact_surfaces_contact_point_position_1_offsets_stride = array_props[1];

				GLenum const offset_label{ GL_OFFSET };

				GLuint const contact_surfaces_tangent_index
				{
					glGetProgramResourceIndex(environment.state.old_triangle_contact_update_shader, GL_BUFFER_VARIABLE, "Contact_Surfaces.contact_surfaces[0].tangent")
				};
				glGetProgramResourceiv
				(
					environment.state.old_triangle_contact_update_shader, GL_BUFFER_VARIABLE, contact_surfaces_tangent_index,
					1u, &offset_label, 1u, nullptr, &environment.state.contact_surface_buffer_contact_surfaces_tangent_offset
				);

				GLuint const contact_surfaces_contact_point_tangent_0_mass_index
				{
					glGetProgramResourceIndex(environment.state.old_triangle_contact_update_shader, GL_BUFFER_VARIABLE, "Contact_Surfaces.contact_surfaces[0].contact_point_tangents[0].mass")
				};
				glGetProgramResourceiv
				(
					environment.state.old_triangle_contact_update_shader, GL_BUFFER_VARIABLE, contact_surfaces_contact_point_tangent_0_mass_index,
					1u, &offset_label, 1u, nullptr, &environment.state.contact_surface_buffer_contact_surfaces_contact_point_tangent_0_mass_offset
				);

				GLuint const contact_surfaces_contact_point_tangent_0_impulse_index
				{
					glGetProgramResourceIndex(environment.state.old_triangle_contact_update_shader, GL_BUFFER_VARIABLE, "Contact_Surfaces.contact_surfaces[0].contact_point_tangents[0].impulse")
				};
				glGetProgramResourceiv
				(
					environment.state.old_triangle_contact_update_shader, GL_BUFFER_VARIABLE, contact_surfaces_contact_point_tangent_0_impulse_index,
					1u, &offset_label, 1u, nullptr, &environment.state.contact_surface_buffer_contact_surfaces_contact_point_tangent_0_impulse_offset
				);

				GLuint const contact_surfaces_contact_point_tangent_1_mass_index
				{
					glGetProgramResourceIndex(environment.state.old_triangle_contact_update_shader, GL_BUFFER_VARIABLE, "Contact_Surfaces.contact_surfaces[0].contact_point_tangents[1].mass")
				};
				glGetProgramResourceiv
				(
					environment.state.old_triangle_contact_update_shader, GL_BUFFER_VARIABLE, contact_surfaces_contact_point_tangent_1_mass_index,
					1u, &offset_label, 1u, nullptr, &environment.state.contact_surface_buffer_contact_surfaces_contact_point_tangent_1_mass_offset
				);

				GLuint const contact_surfaces_contact_point_tangent_1_impulse_index
				{
					glGetProgramResourceIndex(environment.state.old_triangle_contact_update_shader, GL_BUFFER_VARIABLE, "Contact_Surfaces.contact_surfaces[0].contact_point_tangents[1].impulse")
				};
				glGetProgramResourceiv
				(
					environment.state.old_triangle_contact_update_shader, GL_BUFFER_VARIABLE, contact_surfaces_contact_point_tangent_1_impulse_index,
					1u, &offset_label, 1u, nullptr, &environment.state.contact_surface_buffer_contact_surfaces_contact_point_tangent_1_impulse_offset
				);

				GLuint const contact_surfaces_contact_point_normal_0_target_velocity_index
				{
					glGetProgramResourceIndex(environment.state.old_triangle_contact_update_shader, GL_BUFFER_VARIABLE, "Contact_Surfaces.contact_surfaces[0].contact_point_normals[0].target_velocity")
				};
				glGetProgramResourceiv
				(
					environment.state.old_triangle_contact_update_shader, GL_BUFFER_VARIABLE, contact_surfaces_contact_point_normal_0_target_velocity_index,
					1u, &offset_label, 1u, nullptr, &environment.state.contact_surface_buffer_contact_surfaces_contact_point_normal_0_target_velocity_offset
				);

				GLuint const contact_surfaces_contact_point_normal_0_mass_index
				{
					glGetProgramResourceIndex(environment.state.old_triangle_contact_update_shader, GL_BUFFER_VARIABLE, "Contact_Surfaces.contact_surfaces[0].contact_point_normals[0].mass")
				};
				glGetProgramResourceiv
				(
					environment.state.old_triangle_contact_update_shader, GL_BUFFER_VARIABLE, contact_surfaces_contact_point_normal_0_mass_index,
					1u, &offset_label, 1u, nullptr, &environment.state.contact_surface_buffer_contact_surfaces_contact_point_normal_0_mass_offset
				);

				GLuint const contact_surfaces_contact_point_normal_0_impulse_index
				{
					glGetProgramResourceIndex(environment.state.old_triangle_contact_update_shader, GL_BUFFER_VARIABLE, "Contact_Surfaces.contact_surfaces[0].contact_point_normals[0].impulse")
				};
				glGetProgramResourceiv
				(
					environment.state.old_triangle_contact_update_shader, GL_BUFFER_VARIABLE, contact_surfaces_contact_point_normal_0_impulse_index,
					1u, &offset_label, 1u, nullptr, &environment.state.contact_surface_buffer_contact_surfaces_contact_point_normal_0_impulse_offset
				);

				GLuint const contact_surfaces_contact_point_normal_1_target_velocity_index
				{
					glGetProgramResourceIndex(environment.state.old_triangle_contact_update_shader, GL_BUFFER_VARIABLE, "Contact_Surfaces.contact_surfaces[0].contact_point_normals[1].target_velocity")
				};
				glGetProgramResourceiv
				(
					environment.state.old_triangle_contact_update_shader, GL_BUFFER_VARIABLE, contact_surfaces_contact_point_normal_1_target_velocity_index,
					1u, &offset_label, 1u, nullptr, &environment.state.contact_surface_buffer_contact_surfaces_contact_point_normal_1_target_velocity_offset
				);

				GLuint const contact_surfaces_contact_point_normal_1_mass_index
				{
					glGetProgramResourceIndex(environment.state.old_triangle_contact_update_shader, GL_BUFFER_VARIABLE, "Contact_Surfaces.contact_surfaces[0].contact_point_normals[1].mass")
				};
				glGetProgramResourceiv
				(
					environment.state.old_triangle_contact_update_shader, GL_BUFFER_VARIABLE, contact_surfaces_contact_point_normal_1_mass_index,
					1u, &offset_label, 1u, nullptr, &environment.state.contact_surface_buffer_contact_surfaces_contact_point_normal_1_mass_offset
				);

				GLuint const contact_surfaces_contact_point_normal_1_impulse_index
				{
					glGetProgramResourceIndex(environment.state.old_triangle_contact_update_shader, GL_BUFFER_VARIABLE, "Contact_Surfaces.contact_surfaces[0].contact_point_normals[1].impulse")
				};
				glGetProgramResourceiv
				(
					environment.state.old_triangle_contact_update_shader, GL_BUFFER_VARIABLE, contact_surfaces_contact_point_normal_1_impulse_index,
					1u, &offset_label, 1u, nullptr, &environment.state.contact_surface_buffer_contact_surfaces_contact_point_normal_1_impulse_offset
				);

				GLint offsets[]
				{
					environment.state.contact_surface_buffer_contact_surfaces_bodies_offset,
					environment.state.contact_surface_buffer_contact_surfaces_contact_point_position_0_offsets_offset,
					environment.state.contact_surface_buffer_contact_surfaces_contact_point_position_1_offsets_offset,
					environment.state.contact_surface_buffer_contact_surfaces_contact_point_tangent_0_mass_offset,
					environment.state.contact_surface_buffer_contact_surfaces_contact_point_tangent_0_impulse_offset,
					environment.state.contact_surface_buffer_contact_surfaces_contact_point_tangent_1_mass_offset,
					environment.state.contact_surface_buffer_contact_surfaces_contact_point_tangent_1_impulse_offset,
					environment.state.contact_surface_buffer_contact_surfaces_contact_point_normal_0_target_velocity_offset,
					environment.state.contact_surface_buffer_contact_surfaces_contact_point_normal_0_mass_offset,
					environment.state.contact_surface_buffer_contact_surfaces_contact_point_normal_0_impulse_offset,
					environment.state.contact_surface_buffer_contact_surfaces_contact_point_normal_1_target_velocity_offset,
					environment.state.contact_surface_buffer_contact_surfaces_contact_point_normal_1_mass_offset,
					environment.state.contact_surface_buffer_contact_surfaces_contact_point_normal_1_impulse_offset
				};
				environment.state.contact_surface_buffer_contact_surfaces_offset = *std::min_element(std::begin(offsets), std::end(offsets));
			}

#if USE_DYNAMIC_SIZES == true
			environment.state.contact_surface_buffer_size = environment.state.contact_surface_buffer_contact_surfaces_offset + game_logic_MAX_CONTACT_COUNT(environment) * environment.state.contact_surface_buffer_contact_surfaces_stride;
#else
			GLuint const block_index
			{
				glGetProgramResourceIndex(environment.state.old_triangle_contact_update_shader, GL_SHADER_STORAGE_BLOCK, "Contact_Surfaces")
			};
			GLenum const buffer_size_label{ GL_BUFFER_DATA_SIZE };
			glGetProgramResourceiv
			(
				environment.state.old_triangle_contact_update_shader, GL_SHADER_STORAGE_BLOCK, block_index,
				1, &buffer_size_label, 1, nullptr, &environment.state.contact_surface_buffer_size
			);
#endif

			glNamedBufferStorage
			(
				environment.state.contact_surface_buffer, environment.state.contact_surface_buffer_size, nullptr, 
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
				 1u, &offset_label, 1u, nullptr, &environment.state.contact_count_buffer_contact_count_offset
			 );

			 GLuint const block_index
			 {
				 glGetProgramResourceIndex(environment.state.old_triangle_contact_update_shader, GL_UNIFORM_BLOCK, "Contact_Count")
			 };
			 GLenum const buffer_size_label{ GL_BUFFER_DATA_SIZE };
			 glGetProgramResourceiv
			 (
				 environment.state.old_triangle_contact_update_shader, GL_UNIFORM_BLOCK, block_index,
				 1u, &buffer_size_label, 1u, nullptr, &environment.state.contact_count_buffer_size
			 );

			 unsigned char* const initial_contact_count = new unsigned char[environment.state.contact_count_buffer_size];
			 std::memcpy(initial_contact_count + environment.state.contact_count_buffer_contact_count_offset, &environment.state.current_contact_count, sizeof(GLuint));

			 glNamedBufferStorage
			 (
				 environment.state.contact_count_buffer, environment.state.contact_count_buffer_size, initial_contact_count,
				 0u
			 );

			 delete[] initial_contact_count;

			 glBindBufferBase(GL_UNIFORM_BUFFER, game_logic__util_CONTACT_COUNT_BINDING, environment.state.contact_count_buffer);
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
				1u, &offset_label, 1u, nullptr, &environment.state.persistent_contact_count_buffer_persistent_contact_count_offset
			);

			GLuint const block_index
			{
				glGetProgramResourceIndex(environment.state.new_triangle_contact_shader, GL_UNIFORM_BLOCK, "Persistent_Contact_Count")
			};
			GLenum const buffer_size_label{ GL_BUFFER_DATA_SIZE };
			glGetProgramResourceiv
			(
				environment.state.new_triangle_contact_shader, GL_UNIFORM_BLOCK, block_index,
				1u, &buffer_size_label, 1u, nullptr, &environment.state.persistent_contact_count_buffer_size
			);

			unsigned char* const initial_persistent_contact_count = new unsigned char[environment.state.persistent_contact_count_buffer_size];
			std::memcpy(initial_persistent_contact_count + environment.state.persistent_contact_count_buffer_persistent_contact_count_offset, &environment.state.current_contact_count, sizeof(GLuint));

			glNamedBufferStorage
			(
				environment.state.persistent_contact_count_buffer, environment.state.persistent_contact_count_buffer_size, initial_persistent_contact_count,
				0u
			);

			delete[] initial_persistent_contact_count;

			glBindBufferBase(GL_UNIFORM_BUFFER, game_logic__util_PERSISTENT_CONTACT_COUNT_BINDING, environment.state.persistent_contact_count_buffer);
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
					1u, &offset_label, 1u, nullptr, &environment.state.distance_constraint_buffer_count_offset
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
				environment.state.distance_constraint_buffer_distance_constraints_bodies_offset = props[0u];
				environment.state.distance_constraint_buffer_distance_constraints_stride = props[1u];

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
				environment.state.distance_constraint_buffer_distance_constraints_local_points_offset = array_props[0u];
				environment.state.distance_constraint_buffer_distance_constraints_local_points_stride = array_props[1u];

				GLuint const distance_constraints_offsets_index
				{
					glGetProgramResourceIndex(environment.state.update_distance_constraints_shader, GL_BUFFER_VARIABLE, "Distance_Constraints.distance_constraints[0].offsets[0]")
				};
				glGetProgramResourceiv
				(
					environment.state.update_distance_constraints_shader, GL_BUFFER_VARIABLE, distance_constraints_offsets_index,
					std::size(array_prop_labels), array_prop_labels, 2u, nullptr, array_props
				);
				environment.state.distance_constraint_buffer_distance_constraints_offsets_offset = array_props[0u];
				environment.state.distance_constraint_buffer_distance_constraints_offsets_stride = array_props[1u];

				GLuint const distance_constraints_direction_index
				{
					glGetProgramResourceIndex(environment.state.update_distance_constraints_shader, GL_BUFFER_VARIABLE, "Distance_Constraints.distance_constraints[0].direction")
				};
				glGetProgramResourceiv
				(
					environment.state.update_distance_constraints_shader, GL_BUFFER_VARIABLE, distance_constraints_direction_index,
					1u, &offset_label, 1u, nullptr, &environment.state.distance_constraint_buffer_distance_constraints_direction_offset
				);

				GLuint const distance_constraints_max_distance_index
				{
					glGetProgramResourceIndex(environment.state.update_distance_constraints_shader, GL_BUFFER_VARIABLE, "Distance_Constraints.distance_constraints[0].max_distance")
				};
				glGetProgramResourceiv
				(
					environment.state.update_distance_constraints_shader, GL_BUFFER_VARIABLE, distance_constraints_max_distance_index,
					1u, &offset_label, 1u, nullptr, &environment.state.distance_constraint_buffer_distance_constraints_max_distance_offset
				);

				GLuint const distance_constraints_target_velocity_index
				{
					glGetProgramResourceIndex(environment.state.update_distance_constraints_shader, GL_BUFFER_VARIABLE, "Distance_Constraints.distance_constraints[0].target_velocity")
				};
				glGetProgramResourceiv
				(
					environment.state.update_distance_constraints_shader, GL_BUFFER_VARIABLE, distance_constraints_target_velocity_index,
					1u, &offset_label, 1u, nullptr, &environment.state.distance_constraint_buffer_distance_constraints_target_velocity_offset
				);

				GLuint const distance_constraints_mass_index
				{
					glGetProgramResourceIndex(environment.state.update_distance_constraints_shader, GL_BUFFER_VARIABLE, "Distance_Constraints.distance_constraints[0].mass")
				};
				glGetProgramResourceiv
				(
					environment.state.update_distance_constraints_shader, GL_BUFFER_VARIABLE, distance_constraints_mass_index,
					1u, &offset_label, 1u, nullptr, &environment.state.distance_constraint_buffer_distance_constraints_mass_offset
				);

				GLuint const distance_constraints_impulse_index
				{
					glGetProgramResourceIndex(environment.state.update_distance_constraints_shader, GL_BUFFER_VARIABLE, "Distance_Constraints.distance_constraints[0].impulse")
				};
				glGetProgramResourceiv
				(
					environment.state.update_distance_constraints_shader, GL_BUFFER_VARIABLE, distance_constraints_impulse_index,
					1u, &offset_label, 1u, nullptr, &environment.state.distance_constraint_buffer_distance_constraints_impulse_offset
				);

				GLint offsets[]
				{
					environment.state.distance_constraint_buffer_distance_constraints_bodies_offset,
					environment.state.distance_constraint_buffer_distance_constraints_local_points_offset, 
					environment.state.distance_constraint_buffer_distance_constraints_offsets_offset, 
					environment.state.distance_constraint_buffer_distance_constraints_direction_offset, 
					environment.state.distance_constraint_buffer_distance_constraints_max_distance_offset,
					environment.state.distance_constraint_buffer_distance_constraints_target_velocity_offset,
					environment.state.distance_constraint_buffer_distance_constraints_mass_offset,
					environment.state.distance_constraint_buffer_distance_constraints_impulse_offset,
				};
				environment.state.distance_constraint_buffer_distance_constraints_offset = *std::min_element(std::begin(offsets), std::end(offsets));
			}

#if USE_DYNAMIC_SIZES == true
			environment.state.distance_constraint_buffer_size = environment.state.distance_constraint_buffer_distance_constraints_offset + game_logic_MAX_DISTANCE_CONSTRAINT_COUNT(environment) * environment.state.distance_constraint_buffer_distance_constraints_stride;
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

			glNamedBufferStorage
			(
				environment.state.distance_constraint_buffer, environment.state.distance_constraint_buffer_size, nullptr,
				GL_MAP_PERSISTENT_BIT | GL_MAP_READ_BIT | GL_MAP_WRITE_BIT
			);
			
			glClearNamedBufferSubData
			(
				environment.state.distance_constraint_buffer, 
				GL_R32UI, 
				environment.state.distance_constraint_buffer_count_offset, sizeof(GLuint), 
				GL_RED, GL_UNSIGNED_INT, 
				&environment.state.current_distance_constraint_count
			);

			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, game_logic__util_DISTANCE_CONSTRAINT_BINDING, environment.state.distance_constraint_buffer);
		}

		util::proximity::initialize
		(
			environment.state.proximity_tree, game_logic_MAX_LEAF_COUNT(environment), 
			game_logic_MAX_CONTACT_COUNT(environment)
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

		std::cout << "Contact buffer (" << environment.state.contact_buffer << "):" << std::endl;
		std::cout << "size: " << environment.state.contact_buffer_size << std::endl;
		std::cout << "contacts offset: " << environment.state.contact_buffer_contacts_offset << std::endl;
		std::cout << "contacts stride: " << environment.state.contact_buffer_contacts_stride << std::endl;
		std::cout << std::endl;

		std::cout << "Contact surfaces buffer (" << environment.state.contact_surface_buffer << "):" << std::endl;
		std::cout << "size: " << environment.state.contact_surface_buffer_size << std::endl;
		std::cout << "contact surfaces stride: " << environment.state.contact_surface_buffer_contact_surfaces_stride << std::endl;
		std::cout << "contact surfaces bodies offset: " << environment.state.contact_surface_buffer_contact_surfaces_bodies_offset << std::endl;
		std::cout << "contact surfaces bodies stride: " << environment.state.contact_surface_buffer_contact_surfaces_bodies_stride << std::endl;
		std::cout << "contact surfaces contact point position 0 offsets offset: " << environment.state.contact_surface_buffer_contact_surfaces_contact_point_position_0_offsets_offset << std::endl;
		std::cout << "contact surfaces contact point position 0 offsets stride: " << environment.state.contact_surface_buffer_contact_surfaces_contact_point_position_0_offsets_stride << std::endl;
		std::cout << "contact surfaces contact point position 1 offsets offset: " << environment.state.contact_surface_buffer_contact_surfaces_contact_point_position_1_offsets_offset << std::endl;
		std::cout << "contact surfaces contact point position 1 offsets stride: " << environment.state.contact_surface_buffer_contact_surfaces_contact_point_position_1_offsets_stride << std::endl;
		std::cout << "contact surfaces tangent offset: " << environment.state.contact_surface_buffer_contact_surfaces_tangent_offset << std::endl;
		std::cout << "contact surfaces contact point tangent 0 mass offset: " << environment.state.contact_surface_buffer_contact_surfaces_contact_point_tangent_0_mass_offset << std::endl;
		std::cout << "contact surfaces contact point tangent 0 impulse offset: " << environment.state.contact_surface_buffer_contact_surfaces_contact_point_tangent_0_impulse_offset << std::endl;
		std::cout << "contact surfaces contact point tangent 1 mass offset: " << environment.state.contact_surface_buffer_contact_surfaces_contact_point_tangent_1_mass_offset << std::endl;
		std::cout << "contact surfaces contact point tangent 1 impulse offset: " << environment.state.contact_surface_buffer_contact_surfaces_contact_point_tangent_1_impulse_offset << std::endl;
		std::cout << "contact surfaces contact point normal 0 target velocity offset: " << environment.state.contact_surface_buffer_contact_surfaces_contact_point_normal_0_target_velocity_offset << std::endl;
		std::cout << "contact surfaces contact point normal 0 mass offset: " << environment.state.contact_surface_buffer_contact_surfaces_contact_point_normal_0_mass_offset << std::endl;
		std::cout << "contact surfaces contact point normal 0 impulse offset: " << environment.state.contact_surface_buffer_contact_surfaces_contact_point_normal_0_impulse_offset << std::endl;
		std::cout << "contact surfaces contact point normal 1 target velocity offset: " << environment.state.contact_surface_buffer_contact_surfaces_contact_point_normal_1_target_velocity_offset << std::endl;
		std::cout << "contact surfaces contact point normal 1 mass offset: " << environment.state.contact_surface_buffer_contact_surfaces_contact_point_normal_1_mass_offset << std::endl;
		std::cout << "contact surfaces contact point normal 1 impulse offset: " << environment.state.contact_surface_buffer_contact_surfaces_contact_point_normal_1_impulse_offset << std::endl;
		std::cout << "contact surfaces offset: " << environment.state.contact_surface_buffer_contact_surfaces_offset << std::endl;
		std::cout << std::endl;

		std::cout << "Contact count buffer (" << environment.state.contact_count_buffer << "):" << std::endl;
		std::cout << "size: " << environment.state.contact_count_buffer_size << std::endl;
		std::cout << "contact count offset: " << environment.state.contact_count_buffer_contact_count_offset << std::endl;
		std::cout << std::endl;

		std::cout << "Persistent contact count buffer (" << environment.state.persistent_contact_count_buffer << "):" << std::endl;
		std::cout << "size: " << environment.state.persistent_contact_count_buffer_size << std::endl;
		std::cout << "persistent contact count offset: " << environment.state.persistent_contact_count_buffer_persistent_contact_count_offset << std::endl;
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

		std::cout << "Distance constraint buffer (" << environment.state.distance_constraint_buffer << "):" << std::endl;
		std::cout << "size: " << environment.state.distance_constraint_buffer_size << std::endl;
		std::cout << "count offset: " << environment.state.distance_constraint_buffer_count_offset << std::endl;
		std::cout << "distance constraints offset: " << environment.state.distance_constraint_buffer_distance_constraints_offset << std::endl;
		std::cout << "distance constraints stride: " << environment.state.distance_constraint_buffer_distance_constraints_stride << std::endl;
		std::cout << "distance constraints bodies offset: " << environment.state.distance_constraint_buffer_distance_constraints_bodies_offset << std::endl;
		std::cout << "distance constraints local points offset: " << environment.state.distance_constraint_buffer_distance_constraints_local_points_offset << std::endl;
		std::cout << "distance constraints local points stride: " << environment.state.distance_constraint_buffer_distance_constraints_local_points_stride << std::endl;
		std::cout << "distance constraints offsets offset: " << environment.state.distance_constraint_buffer_distance_constraints_offsets_offset << std::endl;
		std::cout << "distance constraints offsets stride: " << environment.state.distance_constraint_buffer_distance_constraints_offsets_stride << std::endl;
		std::cout << "distance constraints direction offset: " << environment.state.distance_constraint_buffer_distance_constraints_direction_offset << std::endl;
		std::cout << "distance constraints max distance offset: " << environment.state.distance_constraint_buffer_distance_constraints_max_distance_offset << std::endl;
		std::cout << "distance constraints target velocity offset: " << environment.state.distance_constraint_buffer_distance_constraints_target_velocity_offset << std::endl;
		std::cout << "distance constraints mass offset: " << environment.state.distance_constraint_buffer_distance_constraints_mass_offset << std::endl;
		std::cout << "distance constraints impulse offset: " << environment.state.distance_constraint_buffer_distance_constraints_impulse_offset << std::endl;

		std::cout << std::endl;

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
		GLuint const solve_contact_velocities_work_group_count
		{
			ceil_div(environment.state.current_contact_count, game_logic__util__rigid_body_WARM_START_CONTACT_IMPULSES_LOCAL_SIZE(environment))
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
				environment.state.rigid_body_velocity_buffer, environment.state.rigid_body_velocity_snapshot_buffer,
				environment.state.rigid_body_velocity_buffer_v_offset, environment.state.rigid_body_velocity_buffer_v_offset,
				environment.state.current_rigid_body_count * environment.state.rigid_body_velocity_buffer_v_stride
			);

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

	void integrate_velocities(game_environment::Environment& environment)
	{
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);	// Updated velocities

		glUseProgram(environment.state.rigid_body_velocity_integration_shader);
		glDispatchCompute
		(
			ceil_div(environment.state.current_rigid_body_count, game_logic__util__rigid_body_VELOCITY_INTEGRATION_LOCAL_SIZE(environment)),
			1u, 1u
		);
	}

	void update_constraints(game_environment::Environment& environment)
	{
		glUseProgram(environment.state.triangle_bounding_box_update_shader);

		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT/* | GL_BUFFER_UPDATE_BARRIER_BIT*/);	// Positions and velocities from velocity integration

		glDispatchCompute
		(
			ceil_div(environment.state.current_triangle_count, game_logic__util__rigid_body_TRIANGLE_BOUNDING_BOX_UPDATE_LOCAL_SIZE(environment)),
			1u, 1u
		);
		// TODO: Make sure GL_BUFFER_UPDATE_BARRIER is the correct flag for clearing the changed leaf count
		glMemoryBarrier(GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT);	// Changed bounding boxes
		GLsync const fence{ glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0u) };
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

		// IMPORTANT TODO: REMOVE WARM STARTING / CONSTRAINT SOLVING FROM CONTACT SURFACE DETECTION
		glUseProgram(environment.state.old_triangle_contact_update_shader);
		glDispatchCompute
		(
			ceil_div(environment.state.current_contact_count, game_logic__util__rigid_body_OLD_TRIANGLE_CONTACT_UPDATE_LOCAL_SIZE(environment)),
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

		// TODO: Make sure this is the correct flag
		//glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT); // Updated persistent contact surfaces

		//glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT); // TODO: Make sure this is the correct flag
		glClearNamedBufferSubData
		(
			environment.state.changed_bounding_box_buffer,
			GL_R32UI,
			environment.state.changed_bounding_box_buffer_size_offset, sizeof(GLuint),
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

			GLuint const old_contact_count{ environment.state.current_contact_count };

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
					--environment.state.current_contact_count;

					if (contact_index != environment.state.current_contact_count)
					{
						// MAYBE TODO: Avoid multiplications by using members that are added/subtracted along with contact count
						glCopyNamedBufferSubData
						(
							environment.state.contact_buffer, environment.state.contact_buffer,
							environment.state.contact_buffer_contacts_offset + environment.state.current_contact_count * environment.state.contact_buffer_contacts_stride,
							environment.state.contact_buffer_contacts_offset + contact_index * environment.state.contact_buffer_contacts_stride,
							sizeof(GLuint[2])
						);
						glCopyNamedBufferSubData
						(
							environment.state.contact_surface_buffer, environment.state.contact_surface_buffer,
							environment.state.contact_surface_buffer_contact_surfaces_offset + environment.state.current_contact_count * environment.state.contact_surface_buffer_contact_surfaces_stride,
							environment.state.contact_surface_buffer_contact_surfaces_offset + contact_index * environment.state.contact_surface_buffer_contact_surfaces_stride,
							environment.state.contact_surface_buffer_contact_surfaces_stride
						);

						game_state::proximity::Contact const& moved_contact
						{
							environment.state.proximity_tree.contacts[environment.state.current_contact_count]
						};
						environment.state.proximity_tree.contacts[contact_index] = moved_contact;

						if (moved_contact.child_0_neighbor_pair.previous == game_logic__util__proximity_NULL_INDEX)
						{
							environment.state.proximity_tree.nodes[moved_contact.leaf_0].contact = contact_index;
						}
						else
						{
							game_state::proximity::Contact& previous{ environment.state.proximity_tree.contacts[moved_contact.child_0_neighbor_pair.previous] };
							GLuint const side{ static_cast<GLuint>(previous.child_1_neighbor_pair.next == environment.state.current_contact_count) };
							previous.neighbor_pairs[side].next = contact_index;
						}
						if (moved_contact.child_0_neighbor_pair.next != game_logic__util__proximity_NULL_INDEX)
						{
							game_state::proximity::Contact& next{ environment.state.proximity_tree.contacts[moved_contact.child_0_neighbor_pair.next] };
							GLuint const side{ static_cast<GLuint>(next.child_1_neighbor_pair.previous == environment.state.current_contact_count) };
							next.neighbor_pairs[side].previous = contact_index;
						}

						if (moved_contact.child_1_neighbor_pair.previous == game_logic__util__proximity_NULL_INDEX)
						{
							environment.state.proximity_tree.nodes[moved_contact.leaf_1].contact = contact_index;
						}
						else
						{
							game_state::proximity::Contact& previous{ environment.state.proximity_tree.contacts[moved_contact.child_1_neighbor_pair.previous] };
							GLuint const side{ static_cast<GLuint>(previous.child_1_neighbor_pair.next == environment.state.current_contact_count) };
							previous.neighbor_pairs[side].next = contact_index;
						}
						if (moved_contact.child_1_neighbor_pair.next != game_logic__util__proximity_NULL_INDEX)
						{
							game_state::proximity::Contact& next{ environment.state.proximity_tree.contacts[moved_contact.child_1_neighbor_pair.next] };
							GLuint const side{ static_cast<GLuint>(next.child_1_neighbor_pair.previous == environment.state.current_contact_count) };
							next.neighbor_pairs[side].previous = contact_index;
						}

						if (next_contact_index == environment.state.current_contact_count)
						{
							next_contact_index = contact_index;
						}
					}
				}
			};

			struct On_Contacts_Removed
			{
				game_environment::Environment& environment;

				void operator()()
				{
					// TODO: Check if this memory barrier is necessary
					glMemoryBarrier(GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT);
					GLsync const fence{ glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0u) };	// Contact copying
					glFlush();

					glClearNamedBufferSubData
					(
						environment.state.persistent_contact_count_buffer,
						GL_R32UI,
						environment.state.persistent_contact_count_buffer_persistent_contact_count_offset, sizeof(GLuint),
						GL_RED_INTEGER, GL_UNSIGNED_INT,
						&environment.state.current_contact_count
					);

					// TODO: Give GPU something heavy to do while we write the new contacts

					environment.state.current_persistent_contact_count = environment.state.current_contact_count;

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
				}
			};

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
					return environment.state.current_contact_count < game_logic_MAX_CONTACT_COUNT(environment);
				}
			};

			struct Add_Contact
			{
				game_environment::Environment& environment;

				GLuint operator()(GLuint const leaf_0_index, GLuint const leaf_1_index)
				{
					GLuint leaf_indices[2]{ leaf_0_index, leaf_1_index };
					// TODO: Do addition between mapping pointer and offset once during initialization
					std::memcpy
					(
						environment.state.contact_mapping + environment.state.contact_buffer_contacts_offset
						+ environment.state.current_contact_count * environment.state.contact_buffer_contacts_stride,
						&leaf_indices,
						sizeof(leaf_indices)
					);
					return environment.state.current_contact_count++;
				}
			};

			struct On_Contacts_Added
			{
				game_environment::Environment const& environment;
				void operator()()
				{
					GLuint const new_contact_count{ environment.state.current_contact_count - environment.state.current_persistent_contact_count };
					glFlushMappedNamedBufferRange
					(
						environment.state.contact_buffer,
						environment.state.contact_buffer_contacts_offset + environment.state.current_persistent_contact_count * environment.state.contact_buffer_contacts_stride,
						new_contact_count * environment.state.contact_buffer_contacts_stride
					);
					glClearNamedBufferSubData
					(
						environment.state.contact_count_buffer,
						GL_R32UI,
						environment.state.contact_count_buffer_contact_count_offset, sizeof(GLuint),
						GL_RED_INTEGER, GL_UNSIGNED_INT,
						&environment.state.current_contact_count
					);
					glUseProgram(environment.state.new_triangle_contact_shader);
					glDispatchCompute
					(
						ceil_div(new_contact_count, game_logic__util__rigid_body_NEW_TRIANGLE_CONTACT_LOCAL_SIZE(environment)),
						1u, 1u
					);
				}
			};
			On_Removing_Contacts_For on_removing_contacts_for{};
			Remove_Contact remove_contact{ environment };
			On_Contacts_Removed on_contacts_removed{ environment };
			On_Adding_Contacts_For on_adding_contacts_for{};
			Contact_Can_Be_Added contact_can_be_added{ environment };
			Add_Contact add_contact{ environment };
			On_Contacts_Added on_contacts_added{ environment };

			glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT); // Updated persistent contact surfaces
			util::proximity::update_contacts
			(
				environment.state.proximity_tree, game_logic_MAX_LEAF_COUNT(environment),
				on_removing_contacts_for, remove_contact, on_contacts_removed,
				on_adding_contacts_for, contact_can_be_added, add_contact, on_contacts_added
			);

			if (environment.state.tick % 120u == 0u)
			{
				std::cout << "Height: " << util::proximity::compute_height
				(
					environment.state.proximity_tree, game_logic_MAX_LEAF_COUNT(environment)
				) << '\n';
				std::cout << "Changed leaf count: " << environment.state.proximity_tree.changed_leaf_count << '\n';
				std::cout << "Contact count: " << old_contact_count << " - " << old_contact_count - environment.state.current_persistent_contact_count << " + " << environment.state.current_contact_count - environment.state.current_persistent_contact_count << " = " << environment.state.current_contact_count << '\n';
				std::cout << std::endl;
			}
		}
	}

	void tick_positions(game_environment::Environment& environment)
	{
		integrate_velocities(environment);
		update_constraints(environment);
	}

	void tick_physics(game_environment::Environment& environment)
	{		
		tick_velocities(environment);
		tick_positions(environment);

		glMemoryBarrier(GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT);
		environment.state.physics_tick_results_fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0u);
		glFlush();

		return;

		// TODO: REMOVE

		//glUseProgram(environment.state.rigid_body_velocity_integration_shader);

		//glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);	// TODO: This could be moved to the end of tick for either future tick or drawing

		//glDispatchCompute
		//(
		//	ceil_div(environment.state.current_rigid_body_count, game_logic__util__rigid_body_VELOCITY_INTEGRATION_LOCAL_SIZE(environment)),
		//	1u, 1u
		//);

		//glUseProgram(environment.state.triangle_bounding_box_update_shader);

		//glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT);	// Positions and velocities from velocity integration

		//glDispatchCompute
		//(
		//	ceil_div(environment.state.current_triangle_count, game_logic__util__rigid_body_TRIANGLE_BOUNDING_BOX_UPDATE_LOCAL_SIZE(environment)),
		//	1u, 1u
		//);
		//glMemoryBarrier(GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT);
		//GLsync const fence{ glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0u) };
		//glFlush();

		///* If direct position correction is used:
		//glCopyNamedBufferSubData
		//(
		//	environment.state.rigid_body_position_buffer, environment.state.rigid_body_position_snapshot_buffer,
		//	environment.state.rigid_body_position_buffer_p_offset, environment.state.rigid_body_position_buffer_p_offset,
		//	environment.state.current_rigid_body_count * environment.state.rigid_body_position_buffer_p_stride
		//);*/

		///* TODO: Snapshot velocities for bounciness
		//glCopyNamedBufferSubData
		//(
		//	environment.state.rigid_body_velocity_buffer, environment.state.rigid_body_velocity_snapshot_buffer,
		//	environment.state.rigid_body_velocity_buffer_v_offset, environment.state.rigid_body_velocity_buffer_v_offset,
		//	environment.state.current_rigid_body_count * environment.state.rigid_body_velocity_buffer_v_stride
		//);*/

		//// TODO: Might not be needed
		//// TODO: Consider making triangle contact update shader write to the snapshot 
		//// so that this memory barrier becomes unnecessary.
		//glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);	// Writes to bounding boxes must occur before constraining the positions and velocities

		//glUseProgram(environment.state.old_triangle_contact_update_shader);
		//glDispatchCompute
		//(
		//	ceil_div(environment.state.current_contact_count, game_logic__util__rigid_body_OLD_TRIANGLE_CONTACT_UPDATE_LOCAL_SIZE(environment)),
		//	1u, 1u
		//);

		//// TODO: Potential one-time CPU operations we must do before 
		//// waiting for the GPU to be done.

		//GLenum fence_status = glClientWaitSync(fence, 0u, 0u);
		//while (fence_status != GL_ALREADY_SIGNALED && fence_status != GL_CONDITION_SATISFIED)
		//{
		//	// TODO: Do something useful but not necessary while we wait. 
		//	// Example: Optimize proximity tree.
		//	fence_status = glClientWaitSync(fence, 0u, 0u);
		//}

		///*switch (fence_status)
		//{
		//	case GL_ALREADY_SIGNALED:
		//		std::cout << "Already signaled" << std::endl;
		//		break;
		//	case GL_CONDITION_SATISFIED:
		//		std::cout << "Condition satisfied" << std::endl;
		//		break;
		//}*/

		//std::memcpy
		//(
		//	&environment.state.proximity_tree.changed_leaf_count,
		//	environment.state.changed_bounding_boxes_mapping + environment.state.changed_bounding_box_buffer_size_offset,
		//	sizeof(GLuint)
		//);
		//// TODO: Fix shader invocations working on out of bounds data
		//// (by skipping over them when iterating here?)
		///*if (size > 10u)
		//{
		//	std::cout << size << std::endl;
		//}*/
		//glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT); // TODO: Make sure this is correct
		//glClearNamedBufferSubData
		//(
		//	environment.state.changed_bounding_box_buffer,
		//	GL_R32UI,
		//	environment.state.changed_bounding_box_buffer_size_offset, sizeof(GLuint),
		//	GL_RED, GL_UNSIGNED_INT,
		//	nullptr
		//);

		//if (util::proximity::is_empty(environment.state.proximity_tree))
		//{

		//}
		//else if (util::proximity::has_single_node(environment.state.proximity_tree, game_logic_MAX_LEAF_COUNT(environment)))
		//{

		//}
		//else
		//{
		//	unsigned char const* index_start{ environment.state.changed_bounding_boxes_mapping + environment.state.changed_bounding_box_buffer_boxes_index_offset };
		//	unsigned char const* min_x_start{ environment.state.changed_bounding_boxes_mapping + environment.state.changed_bounding_box_buffer_boxes_min_x_offset };
		//	unsigned char const* min_y_start{ environment.state.changed_bounding_boxes_mapping + environment.state.changed_bounding_box_buffer_boxes_min_y_offset };
		//	unsigned char const* max_x_start{ environment.state.changed_bounding_boxes_mapping + environment.state.changed_bounding_box_buffer_boxes_max_x_offset };
		//	unsigned char const* max_y_start{ environment.state.changed_bounding_boxes_mapping + environment.state.changed_bounding_box_buffer_boxes_max_y_offset };

		//	for (GLuint i{ 0 }; i < environment.state.proximity_tree.changed_leaf_count; ++i)
		//	{
		//		GLuint index;
		//		GLint min_x, min_y, max_x, max_y;
		//		std::memcpy(&index, index_start, sizeof(GLuint));
		//		std::memcpy(&min_x, min_x_start, sizeof(GLint));
		//		std::memcpy(&min_y, min_y_start, sizeof(GLint));
		//		std::memcpy(&max_x, max_x_start, sizeof(GLint));
		//		std::memcpy(&max_y, max_y_start, sizeof(GLint));

		//		//std::cout << index << ": ";
		//		//util::proximity::print(std::cout, game_state::proximity::Bounding_Box{ min_x, min_y, max_x, max_y }) << std::endl;

		//		util::proximity::change_leaf_of_multinode_tree
		//		(
		//			environment.state.proximity_tree, game_logic_MAX_LEAF_COUNT(environment),
		//			index, i, min_x, min_y, max_x, max_y
		//		);

		//		index_start += environment.state.changed_bounding_box_buffer_boxes_stride;
		//		min_x_start += environment.state.changed_bounding_box_buffer_boxes_stride;
		//		min_y_start += environment.state.changed_bounding_box_buffer_boxes_stride;
		//		max_x_start += environment.state.changed_bounding_box_buffer_boxes_stride;
		//		max_y_start += environment.state.changed_bounding_box_buffer_boxes_stride;

		//		//std::cout << i << ": " << index << std::endl;
		//	}

		//	GLuint const old_contact_count{ environment.state.current_contact_count };

		//	struct On_Removing_Contacts_For
		//	{
		//		void operator()(GLuint leaf_index)
		//		{
		//		}
		//	};

		//	struct Remove_Contact
		//	{
		//		game_environment::Environment& environment;

		//		void operator()(GLuint const contact_index, GLuint& next_contact_index)
		//		{
		//			--environment.state.current_contact_count;

		//			/*std::cout << "Remove contact " << contact_index << std::endl;
		//			std::cout << "Contact count: " << environment.state.current_contact_count << std::endl;
		//			util::proximity::print_leaf_contacts(std::cout, environment.state.proximity_tree, environment.state.proximity_tree.contacts[contact_index].leaf_0);
		//			util::proximity::print_leaf_contacts(std::cout, environment.state.proximity_tree, environment.state.proximity_tree.contacts[contact_index].leaf_1);
		//			std::cout << std::endl;*/

		//			if (contact_index != environment.state.current_contact_count)
		//			{
		//				// MAYBE TODO: Avoid multiplications by using members that are added/subtracted along with contact count
		//				glCopyNamedBufferSubData
		//				(
		//					environment.state.contact_buffer, environment.state.contact_buffer,
		//					environment.state.contact_buffer_contacts_offset + environment.state.current_contact_count * environment.state.contact_buffer_contacts_stride,
		//					environment.state.contact_buffer_contacts_offset + contact_index * environment.state.contact_buffer_contacts_stride,
		//					sizeof(GLuint[2])
		//				);
		//				glCopyNamedBufferSubData
		//				(
		//					environment.state.contact_surface_buffer, environment.state.contact_surface_buffer,
		//					environment.state.contact_surface_buffer_contact_surfaces_offset + environment.state.current_contact_count * environment.state.contact_surface_buffer_contact_surfaces_stride,
		//					environment.state.contact_surface_buffer_contact_surfaces_offset + contact_index * environment.state.contact_surface_buffer_contact_surfaces_stride,
		//					environment.state.contact_surface_buffer_contact_surfaces_stride
		//				);

		//				game_state::proximity::Contact const& moved_contact
		//				{
		//					environment.state.proximity_tree.contacts[environment.state.current_contact_count]
		//				};
		//				environment.state.proximity_tree.contacts[contact_index] = moved_contact;

		//				if (moved_contact.child_0_neighbor_pair.previous == game_logic__util__proximity_NULL_INDEX)
		//				{
		//					environment.state.proximity_tree.nodes[moved_contact.leaf_0].contact = contact_index;
		//				}
		//				else
		//				{
		//					game_state::proximity::Contact& previous{ environment.state.proximity_tree.contacts[moved_contact.child_0_neighbor_pair.previous] };
		//					GLuint const side{ static_cast<GLuint>(previous.child_1_neighbor_pair.next == environment.state.current_contact_count) };
		//					previous.neighbor_pairs[side].next = contact_index;
		//				}
		//				if (moved_contact.child_0_neighbor_pair.next != game_logic__util__proximity_NULL_INDEX)
		//				{
		//					game_state::proximity::Contact& next{ environment.state.proximity_tree.contacts[moved_contact.child_0_neighbor_pair.next] };
		//					GLuint const side{ static_cast<GLuint>(next.child_1_neighbor_pair.previous == environment.state.current_contact_count) };
		//					next.neighbor_pairs[side].previous = contact_index;
		//				}

		//				if (moved_contact.child_1_neighbor_pair.previous == game_logic__util__proximity_NULL_INDEX)
		//				{
		//					environment.state.proximity_tree.nodes[moved_contact.leaf_1].contact = contact_index;
		//				}
		//				else
		//				{
		//					game_state::proximity::Contact& previous{ environment.state.proximity_tree.contacts[moved_contact.child_1_neighbor_pair.previous] };
		//					GLuint const side{ static_cast<GLuint>(previous.child_1_neighbor_pair.next == environment.state.current_contact_count) };
		//					previous.neighbor_pairs[side].next = contact_index;
		//				}
		//				if (moved_contact.child_1_neighbor_pair.next != game_logic__util__proximity_NULL_INDEX)
		//				{
		//					game_state::proximity::Contact& next{ environment.state.proximity_tree.contacts[moved_contact.child_1_neighbor_pair.next] };
		//					GLuint const side{ static_cast<GLuint>(next.child_1_neighbor_pair.previous == environment.state.current_contact_count) };
		//					next.neighbor_pairs[side].previous = contact_index;
		//				}

		//				if (next_contact_index == environment.state.current_contact_count)
		//				{
		//					next_contact_index = contact_index;
		//				}
		//			}
		//		}
		//	};

		//	struct On_Contacts_Removed
		//	{
		//		game_environment::Environment& environment;
		//		//GLuint& new_contacts_start;

		//		void operator()()
		//		{
		//			// TODO: Check if this memory barrier is necessary
		//			glMemoryBarrier(GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT);
		//			GLsync const fence{ glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0u) };	// Contact copying
		//			glFlush();

		//			glClearNamedBufferSubData
		//			(
		//				environment.state.persistent_contact_count_buffer,
		//				GL_R32UI,
		//				environment.state.persistent_contact_count_buffer_persistent_contact_count_offset, sizeof(GLuint),
		//				GL_RED, GL_UNSIGNED_INT,
		//				&environment.state.current_contact_count
		//			);

		//			// TODO: Snapshot velocities
		//			// TODO: Solve persistent contacts

		//			environment.state.current_persistent_contact_count = environment.state.current_contact_count;
		//			//new_contacts_start = environment.state.current_contact_count;

		//			// TODO: Other operations that we let the GPU perform while it is copying contact 
		//			// to old contact positions.

		//			// TODO: Potential one-time CPU operations we must do before 
		//			// waiting for the GPU to be done. Example: Find the first 
		//			// new contact before waiting.

		//			// IMPORTANT TODO: Collect new contacts in temporary array while waiting for GPU

		//			GLenum fence_status = glClientWaitSync(fence, 0u, 0u);
		//			while (fence_status != GL_ALREADY_SIGNALED && fence_status != GL_CONDITION_SATISFIED)
		//			{
		//				// TODO: Do something useful but not necessary while we wait. 
		//				// Example: Optimize proximity tree.
		//				fence_status = glClientWaitSync(fence, 0u, 0u);
		//			}
		//		}
		//	};

		//	struct On_Adding_Contacts_For
		//	{
		//		void operator()(GLuint leaf_index)
		//		{
		//		}
		//	};

		//	struct Contact_Can_Be_Added
		//	{
		//		game_environment::Environment const& environment;

		//		bool operator()(GLuint const leaf_0_index, GLuint const leaf_1_index)
		//		{
		//			return environment.state.current_contact_count < game_logic_MAX_CONTACT_COUNT(environment);
		//		}
		//	};

		//	struct Add_Contact
		//	{
		//		game_environment::Environment& environment;

		//		GLuint operator()(GLuint const leaf_0_index, GLuint const leaf_1_index)
		//		{
		//			GLuint leaf_indices[2]{ leaf_0_index, leaf_1_index };
		//			// TODO: Do addition between mapping pointer and offset once during initialization
		//			std::memcpy
		//			(
		//				environment.state.contact_mapping + environment.state.contact_buffer_contacts_offset
		//				+ environment.state.current_contact_count * environment.state.contact_buffer_contacts_stride,
		//				&leaf_indices,
		//				sizeof(leaf_indices)
		//			);
		//			return environment.state.current_contact_count++;
		//		}
		//	};

		//	struct On_Contacts_Added
		//	{
		//		game_environment::Environment const& environment;
		//		
		//		void operator()()
		//		{
		//			GLuint const new_contact_count{ environment.state.current_contact_count - environment.state.current_persistent_contact_count };
		//			glFlushMappedNamedBufferRange
		//			(
		//				environment.state.contact_buffer,
		//				environment.state.contact_buffer_contacts_offset + environment.state.current_persistent_contact_count * environment.state.contact_buffer_contacts_stride,
		//				new_contact_count * environment.state.contact_buffer_contacts_stride
		//			);
		//			glClearNamedBufferSubData
		//			(
		//				environment.state.contact_count_buffer,
		//				GL_R32UI,
		//				environment.state.contact_count_buffer_contact_count_offset, sizeof(GLuint),
		//				GL_RED, GL_UNSIGNED_INT,
		//				&environment.state.current_contact_count
		//			);
		//			glUseProgram(environment.state.new_triangle_contact_shader);
		//			glDispatchCompute
		//			(
		//				ceil_div(new_contact_count, game_logic__util__rigid_body_NEW_TRIANGLE_CONTACT_LOCAL_SIZE(environment)),
		//				1u, 1u
		//			);
		//		}
		//	};
		//	//GLuint new_contacts_start;
		//	On_Removing_Contacts_For on_removing_contacts_for{};
		//	Remove_Contact remove_contact{ environment };
		//	On_Contacts_Removed on_contacts_removed{ environment };
		//	On_Adding_Contacts_For on_adding_contacts_for{};
		//	Contact_Can_Be_Added contact_can_be_added{ environment };
		//	Add_Contact add_contact{ environment };
		//	On_Contacts_Added on_contacts_added{ environment };

		//	glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT); // Updated persistent contact surfaces
		//	util::proximity::update_contacts
		//	(
		//		environment.state.proximity_tree, game_logic_MAX_LEAF_COUNT(environment),
		//		on_removing_contacts_for, remove_contact, on_contacts_removed,
		//		on_adding_contacts_for, contact_can_be_added, add_contact, on_contacts_added
		//	);

		//	glUseProgram(environment.state.solve_contact_velocities_shader);

		//	GLuint const solve_contact_velocities_work_group_count
		//	{
		//		ceil_div(environment.state.current_contact_count, game_logic__util__rigid_body_SOLVE_CONTACT_VELOCITIES_LOCAL_SIZE(environment))
		//	};

		//	for (GLuint i{ 0u }; i < 4u; ++i)
		//	{
		//		glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT); // Updated velocities from previous velocity solve

		//		glCopyNamedBufferSubData
		//		(
		//			environment.state.rigid_body_velocity_buffer, environment.state.rigid_body_velocity_snapshot_buffer,
		//			environment.state.rigid_body_velocity_buffer_v_offset, environment.state.rigid_body_velocity_buffer_v_offset,
		//			environment.state.current_rigid_body_count * environment.state.rigid_body_velocity_buffer_v_stride
		//		);

		//		glDispatchCompute(solve_contact_velocities_work_group_count, 1u, 1u);
		//	}

		//	if (environment.state.tick % 120u == 0u)
		//	{

		//		std::cout << "Height: " << util::proximity::compute_height
		//		(
		//			environment.state.proximity_tree, game_logic_MAX_LEAF_COUNT(environment)
		//		) << '\n';
		//		std::cout << "Changed leaf count: " << environment.state.proximity_tree.changed_leaf_count << '\n';
		//		//std::cout << "Contact count: " << old_contact_count << " - " << old_contact_count - environment.state.current_persistent_contact_count << " + " << environment.state.current_contact_count - new_contacts_start << " = " << environment.state.current_contact_count << '\n';
		//		std::cout << std::endl;
		//	}
		//}
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
		game_state::rigid_body::Triangle const& triangle{ environment.state.triangles[triangle_index] };
		util::rigid_body::Position body_position;
		std::memcpy
		(
			&body_position,
			environment.state.position_mapping + environment.state.rigid_body_position_buffer_p_offset + triangle.body * environment.state.rigid_body_position_buffer_p_stride,
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

		GLfloat vertex_0_x{ environment.state.vertices[triangle.vertices[0u]][0u] };
		GLfloat vertex_0_y{ environment.state.vertices[triangle.vertices[0u]][1u] };

		GLfloat vertex_1_x{ environment.state.vertices[triangle.vertices[1u]][0u] };
		GLfloat vertex_1_y{ environment.state.vertices[triangle.vertices[1u]][1u] };

		GLfloat vertex_2_x{ environment.state.vertices[triangle.vertices[2u]][0u] };
		GLfloat vertex_2_y{ environment.state.vertices[triangle.vertices[2u]][1u] };

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
				if (!environment.state.point_grabbed)
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

							void operator()(GLuint const leaf_index, game_state::proximity::Node const& leaf)
							{
								game_state::rigid_body::Triangle const& triangle{ environment.state.triangles[leaf_index] };
								GLint body_position[4u];
								std::memcpy
								(
									&body_position,
									environment.state.position_mapping + environment.state.rigid_body_position_buffer_p_offset + triangle.body * environment.state.rigid_body_position_buffer_p_stride,
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
								&environment.state.triangles[hovered_triangle].body
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
				break;
			case GLFW_RELEASE:
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
				glfwSetCursor(environment.window, environment.state.grab_cursor);
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

							void operator()(GLuint const leaf_index, game_state::proximity::Node const& leaf)
							{
								game_state::rigid_body::Triangle const& triangle{ environment.state.triangles[leaf_index] };
								GLint body_position[4u];
								std::memcpy
								(
									&body_position,
									environment.state.position_mapping + environment.state.rigid_body_position_buffer_p_offset + triangle.body * environment.state.rigid_body_position_buffer_p_stride,
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

							GLuint const count_jump{ environment.state.current_distance_constraint_count * environment.state.distance_constraint_buffer_distance_constraints_stride };
							glClearNamedBufferSubData
							(
								environment.state.distance_constraint_buffer,
								GL_R32UI,
								environment.state.distance_constraint_buffer_distance_constraints_bodies_offset + count_jump, 
								sizeof(GLuint),
								GL_RED_INTEGER, GL_UNSIGNED_INT,
								&environment.state.triangles[hovered_triangle].body
							);
							// TODO: Put hovered_local_x, hovered_local_y in array from start
							GLfloat local_point[2u]
							{
								game_logic__util__spatial_TO_METERS(environment, hovered_local_x),
								game_logic__util__spatial_TO_METERS(environment, hovered_local_y)
							};
							glClearNamedBufferSubData
							(
								environment.state.distance_constraint_buffer,
								GL_RG32F,
								environment.state.distance_constraint_buffer_distance_constraints_local_points_offset + count_jump, 
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
				break;
			case GLFW_RELEASE:
				if (!environment.state.point_grabbed)
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

							void operator()(GLuint const leaf_index, game_state::proximity::Node const& leaf)
							{
								game_state::rigid_body::Triangle const& triangle{ environment.state.triangles[leaf_index] };
								GLint body_position[4u];
								std::memcpy
								(
									&body_position,
									environment.state.position_mapping + environment.state.rigid_body_position_buffer_p_offset + triangle.body * environment.state.rigid_body_position_buffer_p_stride,
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

							GLuint const count_jump{ environment.state.current_distance_constraint_count * environment.state.distance_constraint_buffer_distance_constraints_stride };
							glClearNamedBufferSubData
							(
								environment.state.distance_constraint_buffer, 
								GL_R32UI, 
								environment.state.distance_constraint_buffer_distance_constraints_bodies_offset + 4u + count_jump,
								sizeof(GLuint),
								GL_RED_INTEGER, GL_UNSIGNED_INT,
								&environment.state.triangles[hovered_triangle].body
							);
							// TODO: Put hovered_local_x, hovered_local_y in array from start
							GLfloat local_point[2u]
							{
								game_logic__util__spatial_TO_METERS(environment, hovered_local_x),
								game_logic__util__spatial_TO_METERS(environment, hovered_local_y)
							};
							glClearNamedBufferSubData
							(
								environment.state.distance_constraint_buffer,
								GL_RG32F,
								environment.state.distance_constraint_buffer_distance_constraints_local_points_offset + environment.state.distance_constraint_buffer_distance_constraints_local_points_stride + count_jump,
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

	bool triangle_contains_point
	(
		game_environment::Environment const& environment, GLuint const triangle_index, 
		GLint x, GLint y
	)
	{
		game_state::rigid_body::Triangle const& triangle{ environment.state.triangles[triangle_index] };
		util::rigid_body::Position body_position;
		std::memcpy
		(
			&body_position,
			environment.state.position_mapping + environment.state.rigid_body_position_buffer_p_offset + triangle.body * environment.state.rigid_body_position_buffer_p_stride,
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

		GLfloat vertex_0_x{ environment.state.vertices[triangle.vertices[0u]][0u] };
		GLfloat vertex_0_y{ environment.state.vertices[triangle.vertices[0u]][1u] };

		GLfloat vertex_1_x{ environment.state.vertices[triangle.vertices[1u]][0u] };
		GLfloat vertex_1_y{ environment.state.vertices[triangle.vertices[1u]][1u] };

		GLfloat vertex_2_x{ environment.state.vertices[triangle.vertices[2u]][0u] };
		GLfloat vertex_2_y{ environment.state.vertices[triangle.vertices[2u]][1u] };

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
		glClear(GL_COLOR_BUFFER_BIT);

		update_GPU_camera(environment);

		// TODO: Indirect drawing
		
		//glUseProgram(environment.state.shader);
		//glBindVertexArray(environment.state.vao);
		//glDrawArrays(GL_TRIANGLES, 0, 6);

		glUseProgram(environment.state.triangle_draw_shader);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
		glDrawArrays(GL_TRIANGLES, 0, environment.state.current_triangle_count * 3u);

		//glUseProgram(environment.state.triangle_wireframes_draw_shader);
		//glDrawArrays(GL_LINES, 0, environment.state.current_triangle_count * 6u);

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

						void operator()(GLuint const leaf_index, game_state::proximity::Node const& leaf)
						{
							game_state::rigid_body::Triangle const& triangle{ environment.state.triangles[leaf_index] };
							GLint body_position[4u];
							std::memcpy
							(
								&body_position,
								environment.state.position_mapping + environment.state.rigid_body_position_buffer_p_offset + triangle.body * environment.state.rigid_body_position_buffer_p_stride,
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

						glfwSetCursor(environment.window, environment.state.point_cursor);
					}
					else
					{
						glfwSetCursor(environment.window, nullptr);
					}
				}
			}
		}

		//glUseProgram(environment.state.rigid_body_debug_rendering_shader);
		//glDrawArrays(GL_LINES, 0, environment.state.current_rigid_body_count * 4u);

		//glUseProgram(environment.state.triangle_normal_draw_shader);
		//glDrawArrays(GL_LINES, 0, environment.state.current_triangle_count * 6u);

		//glUseProgram(environment.state.triangle_bounding_box_draw_shader);
		//glDrawArrays(GL_LINES, 0, environment.state.current_triangle_count * 8u);

		/*if (!util::proximity::is_empty(environment.state.proximity_tree))
		{
			glUseProgram(environment.state.parent_bounding_box_draw_shader);
			draw_inner_bounding_boxes(environment, environment.state.proximity_tree.root);
		}*/

		//glUseProgram(environment.state.leaf_contact_draw_shader);
		//glDrawArrays(GL_LINES, 0, environment.state.current_contact_count * 2u);

		//glUseProgram(environment.state.contact_point_positions_draw_shader);
		//glPointSize(10.0f);
		//glDrawArrays(GL_POINTS, 0, environment.state.current_contact_count * 4u);

		//glUseProgram(environment.state.contact_point_offsets_draw_shader);
		//glDrawArrays(GL_LINES, 0, environment.state.current_contact_count * 8u);

		//glUseProgram(environment.state.contact_basis_draw_shader);
		//glDrawArrays(GL_LINES, 0, environment.state.current_contact_count * 4u);

		//glUseProgram(environment.state.contact_impulses_draw_shader);
		//glDrawArrays(GL_LINES, 0, environment.state.current_contact_count * 16u);

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
		glUnmapNamedBuffer(environment.state.rigid_body_position_buffer);
		glUnmapNamedBuffer(environment.state.changed_bounding_box_buffer);
		glUnmapNamedBuffer(environment.state.contact_buffer);

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
			environment.state.changed_bounding_box_buffer, 
			environment.state.contact_buffer, 
			environment.state.contact_surface_buffer, 
			environment.state.contact_count_buffer, 
			environment.state.persistent_contact_count_buffer, 
			environment.state.rigid_body_velocity_snapshot_buffer, 
			environment.state.rigid_body_position_snapshot_buffer, 
			environment.state.cursor_position_buffer, 
			environment.state.cursor_constrained_point_buffer, 
			environment.state.cursor_constraint_buffer, 
			environment.state.distance_constraint_buffer
		};
		glDeleteBuffers(std::size(buffers), buffers);

		glDeleteVertexArrays(1, &environment.state.vao);
		glDeleteBuffers(1, &environment.state.vbo);

		glfwDestroyCursor(environment.state.grab_cursor);
		glfwDestroyCursor(environment.state.point_cursor);

		// TODO: Delete shader programs???
	}
}
