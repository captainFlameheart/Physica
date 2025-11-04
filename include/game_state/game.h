#pragma once
#include "glad_glfw.h"
#include "game_state/camera/camera.h"
#include "util/math/vector_2D.h"
#include "game_state/proximity/Tree.h"
#include "game_state/rigid_body/Triangle.h"
#include "game_state/cursor_types/cursor_type_set.h"
#include "GPU_buffers/include.h"

#define MAX_GRAVITY_SOURCE_COUNT 100u

template <unsigned int Vertex_Index_Count>
struct Model
{
	GLuint vertex_indices[Vertex_Index_Count];
	GLfloat inverse_mass;
	GLfloat inverse_inertia;
};

namespace game_state
{

	// TODO: Organize based on access patterns 
	// (initialize -> render -> events -> tick * n -> render -> events -> tick * n -> render -> ...)
	struct Game
	{
		GLuint time_elapsed_query;

		cursor_types::Cursor_Type_Set cursor_types;

		camera::Camera camera;
		GLboolean point_grabbed;
		util::math::Vector_2D grabbed_point;

		// TODO: Camera buffer should be after all tick buffers
		unsigned char* camera_send_buffer;	// TODO: Remove through persistent memory mapping
		GLuint camera_buffer;
		GLint camera_buffer_size;
		GLint camera_buffer_xy_offset;
		GLint camera_buffer_angle_offset;
		GLint camera_buffer_z_offset;
		GLint camera_buffer_view_rotation_offset;
		GLint camera_buffer_view_rotation_stride;
		GLuint shader;
		GLuint vao;
		GLuint vbo;

		GLuint current_rigid_body_count;

		GPU_buffers::GPU_Buffer_Set GPU_buffers;

		GLint current_fluid_particle_count;
		
		GLuint current_distance_constraint_count;
		GLuint add_distance_constraint_shader;

		GLuint current_triangle_contact_count;

		GLuint persist_fluid_contacts_shader;
		GLuint persist_fluid_triangle_contacts_shader;
		GLuint old_triangle_contact_update_shader;

		GLuint new_fluid_contacts_shader;
		GLuint new_fluid_triangle_contacts_shader;
		GLuint new_fluid_triangle_contacts_shader_persistent_count_uniform_location;
		GLint new_fluid_contacts_shader_persistent_count_uniform_location;
		GLuint new_triangle_contact_shader;

		GLuint update_distance_constraints_shader;

		GLuint warm_start_fluid_contacts_shader;
		GLuint warm_start_fluid_triangle_contacts_shader;
		GLuint warm_start_contact_impulses_shader;
		GLuint warm_start_distance_constraints_shader;
		GLuint update_and_warm_start_cursor_constraint_shader;

		GLuint solve_fluid_contacts_shader;
		GLuint solve_fluid_triangle_contacts_shader;
		GLuint solve_contact_velocities_shader;
		GLuint solve_distance_constraints_shader;
		GLuint solve_cursor_constraint_shader;

		proximity::Tree proximity_tree;

		// TODO: Consider using std::430 layout to improve runtime performance
		GLuint current_persistent_contact_count;
		GLuint contact_buffer;
		GLint contact_buffer_size;
		GLint contact_buffer_contacts_offset;
		GLint contact_buffer_contacts_stride;

		GLuint cursor_position_buffer;
		GLint cursor_position_buffer_size;
		GLint cursor_position_buffer_position_offset;

		GLuint cursor_constrained_point_buffer;
		GLint cursor_constrained_point_buffer_size;
		GLint cursor_constrained_point_buffer_body_offset;
		GLint cursor_constrained_point_buffer_local_point_offset;

		GLuint cursor_constraint_buffer;
		GLint cursor_constraint_buffer_size;
		GLint cursor_constraint_buffer_offset_offset;
		GLint cursor_constraint_buffer_target_velocity_offset;
		GLint cursor_constraint_buffer_mass_offset;
		GLint cursor_constraint_buffer_mass_matrix_stride;
		GLint cursor_constraint_buffer_impulse_offset;

		GLuint gravity_sources_buffer;
		GLint gravity_sources_buffer_size;
		GLint gravity_sources_buffer_count_offset;
		GLint gravity_sources_buffer_positions_offset;
		GLint gravity_sources_buffer_positions_stride;
		GLint gravity_sources_buffer_strengths_offset;
		GLint gravity_sources_buffer_strengths_stride;

		GLuint current_gravity_source_count;
		util::math::Vector_2D gravity_source_positions[MAX_GRAVITY_SOURCE_COUNT];
		GLfloat gravity_source_strengths[MAX_GRAVITY_SOURCE_COUNT];
		GLuint grabbed_gravity_source;

		GLuint count_buffer;
		GLint count_buffer_size;
		GLint count_buffer_bodies_offset;
		GLint count_buffer_triangles_offset;
		GLint count_buffer_fluid_particles_offset;

		GLuint test_compute_shader;

		GLuint rigid_body_velocity_integration_shader;
		GLuint integrate_fluid_velocity_shader;
		GLuint triangle_bounding_box_update_shader;

		unsigned char* changed_bounding_boxes_mapping;
		
		// TODO: Consider using std::430 layout to improve runtime performance
		GLuint contact_surface_buffer;
		GLint contact_surface_buffer_contact_surfaces_offset;
		GLint contact_surface_buffer_contact_surfaces_stride;

		unsigned char* contact_mapping;

		GLsync physics_tick_results_fence;

		GLuint grabbed_triangle;
		GLuint distance_constraint_start_triangle;

		GLuint tick;
		bool physics_running;

		GLuint fluid_texture;
		GLuint fluid_framebuffer;

		GLuint fluid_particles_draw_shader;

		bool debug_fluid_particles_visible;
		GLuint debug_fluid_particles_draw_shader;

		GLuint fluid_draw_shader;
		GLint fluid_draw_shader_fluid_texture_uniform_location;

		GLuint triangle_draw_shader;

		bool triangle_wireframes_visible;
		GLuint triangle_wireframes_draw_shader;
		
		GLuint hovered_triangle_wireframe_draw_shader;
		GLint hovered_triangle_wireframe_hovered_triangle_uniform_location;

		bool rigid_bodies_visible;
		GLuint rigid_body_debug_rendering_shader;

		bool triangle_normals_visible;
		GLuint triangle_normal_draw_shader;
		
		bool leaf_bounding_boxes_visible;
		GLuint triangle_bounding_box_draw_shader;
		GLuint fluid_bounding_boxes_draw_shader;

		bool parent_bounding_boxes_visible;
		GLuint parent_bounding_box_draw_shader;
		GLint parent_bounding_box_uniform_location;

		bool leaf_contacts_visible;
		GLuint fluid_leaf_contacts_draw_shader;
		GLuint fluid_triangle_leaf_contacts_draw_shader;
		GLuint leaf_triangle_contact_draw_shader;

		bool contact_point_positions_visible;
		GLuint fluid_triangle_contact_points_draw_shader;
		GLuint contact_point_positions_draw_shader;

		GLuint contact_point_offsets_draw_shader;
		
		bool contact_basis_visible;
		GLuint fluid_triangle_contact_basis_draw_shader;
		GLuint contact_basis_draw_shader;
		
		bool contact_impulses_visible;
		GLuint contact_impulses_draw_shader;

		bool gravity_visible;
		GLuint gravity_grid_points_draw_shader;
		GLuint gravity_directions_draw_shader;

		GLuint gravity_sources_draw_shader;
		GLuint hovered_gravity_source_wireframe_draw_shader;
		GLuint hovered_gravity_source_wireframe_draw_shader_hovered_gravity_source_uniform_location;

		GLuint distance_constraints_draw_shader;

		GLuint distance_constraint_start_draw_shader;
		
		GLuint cursor_constraint_draw_shader;
		
		GLuint cursor_position_draw_shader;
	};
}
