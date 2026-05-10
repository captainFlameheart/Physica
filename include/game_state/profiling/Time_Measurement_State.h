#pragma once
#include "glad_glfw.h"
#include <string>

namespace game_state::profiling
{
	enum Timestamp_Type : GLuint
	{
		individual_command,
		phase,
		tick,
		draw_source,
		illuminate_source,
		draw,
		frame,

		count
	};

	struct Timestamp_Metadata
	{
		Timestamp_Type type;
		GLuint previous;
		GLuint next;
		std::string name;
	};

	struct Time_Measurement_State
	{
		GLuint timestamp_capacity;
		GLuint query_capacity;

		GLuint measured_type_count;
		bool measured_types[Timestamp_Type::count];
		GLuint previous_timestamps[Timestamp_Type::count];

		GLuint timestamp_base;
		GLuint next;
		GLuint base_query;
		GLuint query_count;

		GLuint buffer;
		GLuint* queries;
		Timestamp_Metadata* timestamp_metadata;
	};
}
