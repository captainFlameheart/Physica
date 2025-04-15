#pragma once
#include "glad_glfw.h"
#include "game_state/camera/camera.h"
#include "util/math/vector_2D.h"

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

		GLuint changed_bounding_box_buffer;
		GLint changed_bounding_box_buffer_size;
		GLint changed_bounding_box_buffer_size_offset;
		GLint changed_bounding_box_buffer_boxes_index_offset;
		GLint changed_bounding_box_buffer_boxes_stride;

		GLuint rigid_body_velocity_integration_shader;
		GLuint triangle_bounding_box_update_shader;
		unsigned char* changed_bounding_boxes_mapping;

		GLuint triangle_draw_shader;
		GLuint triangle_bounding_box_draw_shader;
		GLuint rigid_body_debug_rendering_shader;
	};
}
