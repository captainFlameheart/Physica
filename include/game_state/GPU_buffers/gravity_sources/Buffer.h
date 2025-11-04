#pragma once

#define MAX_GRAVITY_SOURCE_COUNT 100u

namespace game_state::GPU_buffers::gravity_sources
{
	struct Buffer
	{
		GLuint buffer;
		GLint size;
		GLint count_offset;
		GLint positions_offset;
		GLint positions_stride;
		GLint strengths_offset;
		GLint strengths_stride;

		GLuint current_gravity_source_count;
		util::math::Vector_2D gravity_source_positions[MAX_GRAVITY_SOURCE_COUNT];
		GLfloat gravity_source_strengths[MAX_GRAVITY_SOURCE_COUNT];
		GLuint grabbed_gravity_source;
	};
}
