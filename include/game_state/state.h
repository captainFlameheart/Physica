#pragma once
#include "glad_glfw.h"
#include "game/camera/camera.h"
#include "util/math/vector_2D.h"

namespace game_state
{
	struct State
	{
		GLFWcursor* grab_cursor;

		game::camera::Camera camera;
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
	};
}
