#pragma once

#include "game/camera/camera.h"

namespace game_state
{
	struct State
	{
		game::camera::Camera camera;
		unsigned char* camera_send_buffer;	// TODO: Remove through persistent memory mapping
		GLuint camera_buffer;
		GLint camera_buffer_size;
		GLint camera_buffer_xy_offset;
		GLint camera_buffer_right_offset;
		GLint camera_buffer_z_offset;
		GLuint shader;
		GLuint vao;
		GLuint vbo;
	};
}
