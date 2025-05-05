#pragma once
#include "glad_glfw.h"
#include "game_state/camera/camera.h"
#include "util/math/vector_2D.h"
#include "game_state/proximity/Tree.h"

namespace game_state
{
	// TODO: Organize based on access patterns 
	// (initialize -> render -> events -> tick * n -> render -> events -> tick * n -> render -> ...)
	struct Game
	{
		GLFWcursor* grab_cursor;

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

		// TODO: Position buffer should likely come after velocity buffer

		GLuint rigid_body_position_buffer;
		GLint rigid_body_position_buffer_size;
		GLint rigid_body_position_buffer_p_offset;
		GLint rigid_body_position_buffer_p_stride;
		
		GLuint rigid_body_velocity_buffer;
		GLint rigid_body_velocity_buffer_size;
		GLint rigid_body_velocity_buffer_v_offset;
		GLint rigid_body_velocity_buffer_v_stride;

		GLuint current_triangle_count;

		GLuint triangle_buffer;
		GLint triangle_buffer_size;
		GLint triangle_buffer_triangles_offset;
		GLint triangle_buffer_triangles_stride;

		GLuint vertex_buffer;
		GLint vertex_buffer_size;
		GLint vertex_buffer_vertices_offset;
		GLint vertex_buffer_vertices_stride;

		GLuint bounding_box_buffer;
		GLint bounding_box_buffer_size;
		GLint bounding_box_buffer_boxes_offset;
		GLint bounding_box_buffer_boxes_stride;

		GLint contact_surface_buffer_size;

		GLint contact_surface_buffer_contact_surfaces_bodies_offset;
		GLint contact_surface_buffer_contact_surfaces_bodies_stride;

		GLint contact_surface_buffer_contact_surfaces_contact_point_position_0_offsets_offset;
		GLint contact_surface_buffer_contact_surfaces_contact_point_position_0_offsets_stride;
		GLint contact_surface_buffer_contact_surfaces_contact_point_position_1_offsets_offset;
		GLint contact_surface_buffer_contact_surfaces_contact_point_position_1_offsets_stride;

		GLint contact_surface_buffer_contact_surfaces_tangent_offset;
		GLint contact_surface_buffer_contact_surfaces_contact_point_tangent_0_mass_offset;
		GLint contact_surface_buffer_contact_surfaces_contact_point_tangent_0_impulse_offset;
		GLint contact_surface_buffer_contact_surfaces_contact_point_tangent_1_mass_offset;
		GLint contact_surface_buffer_contact_surfaces_contact_point_tangent_1_impulse_offset;

		GLint contact_surface_buffer_contact_surfaces_contact_point_normal_0_mass_offset;
		GLint contact_surface_buffer_contact_surfaces_contact_point_normal_0_impulse_offset;
		GLint contact_surface_buffer_contact_surfaces_contact_point_normal_1_mass_offset;
		GLint contact_surface_buffer_contact_surfaces_contact_point_normal_1_impulse_offset;
		
		GLint contact_count_buffer;
		GLint contact_count_buffer_size;
		GLint contact_count_buffer_contact_count_offset;

		GLint persistent_contact_count_buffer;
		GLint persistent_contact_count_buffer_size;
		GLint persistent_contact_count_buffer_persistent_contact_count_offset;

		// TODO: Consider using std::430 layout to improve runtime performance
		GLuint changed_bounding_box_buffer;
		GLint changed_bounding_box_buffer_size;
		GLint changed_bounding_box_buffer_size_offset;
		GLint changed_bounding_box_buffer_boxes_stride;
		GLint changed_bounding_box_buffer_boxes_index_offset;
		GLint changed_bounding_box_buffer_boxes_min_x_offset;
		GLint changed_bounding_box_buffer_boxes_min_y_offset;
		GLint changed_bounding_box_buffer_boxes_max_x_offset;
		GLint changed_bounding_box_buffer_boxes_max_y_offset;

		GLuint old_triangle_contact_update_shader;

		GLuint new_triangle_contact_shader;

		proximity::Tree proximity_tree;

		// TODO: Consider using std::430 layout to improve runtime performance
		GLuint current_contact_count;
		GLuint contact_buffer;
		GLint contact_buffer_size;
		GLint contact_buffer_contacts_offset;
		GLint contact_buffer_contacts_stride;

		GLuint rigid_body_velocity_integration_shader;
		GLuint triangle_bounding_box_update_shader;
		unsigned char* changed_bounding_boxes_mapping;
		
		// TODO: Consider using std::430 layout to improve runtime performance
		GLuint contact_surface_buffer;
		GLint contact_surface_buffer_contact_surfaces_offset;
		GLint contact_surface_buffer_contact_surfaces_stride;

		unsigned char* contact_mapping;

		GLuint tick;
		bool physics_running;

		GLuint triangle_draw_shader;
		GLuint triangle_bounding_box_draw_shader;
		GLuint parent_bounding_box_draw_shader;
		GLint parent_bounding_box_uniform_location;
		GLuint leaf_contact_draw_shader;
		GLuint contact_point_positions_draw_shader;
		GLuint contact_point_offsets_draw_shader;
		GLuint rigid_body_debug_rendering_shader;
	};
}
