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

		GLuint rigid_body_position_buffer;
		GLint rigid_body_position_buffer_size;
		GLint rigid_body_position_buffer_p_offset;
		GLint rigid_body_position_buffer_p_stride;
		
		GLuint rigid_body_velocity_buffer;
		GLint rigid_body_velocity_buffer_size;
		GLint rigid_body_velocity_buffer_v_offset;
		GLint rigid_body_velocity_buffer_v_stride;

		GLuint rigid_body_velocity_integration_shader;

		GLuint rigid_body_debug_rendering_shader;
	};
}
