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
		bool is_start;
		std::string name;
	};

	struct Time_Measurement_State
	{
		GLuint timestamp_capacity;

		GLuint measured_type_count;
		bool measured_types[Timestamp_Type::count];
		GLuint previous_timestamps[Timestamp_Type::count];

		GLuint next;

		GLuint buffer;
		Timestamp_Metadata* timestamp_metadata;
	};
}
