#pragma once
#include "glad_glfw.h"
#include <string>
#include <string_view>
#include <array>
using namespace std::string_view_literals;

namespace game_state::profiling
{
	struct Timestamp_Metadata1
	{
		GLuint generation;
		GLfloat color[4u];
		std::string name;
	};

	struct Timing
	{
		GLuint timestamp_capacity;
		GLuint generation_capacity;
		GLuint query_capacity;

		GLuint generation;
		GLuint next_timestamp;
		GLuint query_count;

		union
		{
			struct
			{
				GLuint buffer;
				GLuint metadata_buffer;
			};
			GLuint buffers[2u];
		};
		
		GLuint* queries;
		
		Timestamp_Metadata1* metadata;
		GLuint* generation_start_timestamps;
	};
}
