#pragma once

namespace game_state
{
	struct State
	{
		GLuint vbo;
		GLuint projection_buffer;
		GLuint camera_buffer;
		GLuint shader;
		GLuint vao;
	};
}
