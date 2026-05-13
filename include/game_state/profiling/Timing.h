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
		GLuint metadata_stage_capacity;
		GLuint generation_capacity;
		GLuint query_capacity;
		GLuint generation_query_capacity;

		GLuint generation;
		GLuint next_timestamp;
		GLuint query_count;
		GLuint generation_query_count;

		union
		{
			struct
			{
				GLuint buffer;
				GLuint metadata_buffer;
			};
			GLuint buffers[2u];
		};

		GLbyte* metadata_mapping;
		GLsync metadata_fences[metadata_fence_count];
		GLuint metadata_fence_timestamps[metadata_fence_count];

		GLuint* queries;
		GLuint* generation_queries;
		
		Timestamp_Metadata1* metadata;
		GLuint* generation_start_timestamps;
	};
}
