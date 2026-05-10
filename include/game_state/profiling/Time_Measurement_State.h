#pragma once
#include "glad_glfw.h"
#include <string>
#include <string_view>
#include <array>
using namespace std::string_view_literals;

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

	constexpr std::array<std::string_view, static_cast<GLuint>(Timestamp_Type::count)> initialize_timestamp_type_names()
	{
		std::array<std::string_view, static_cast<GLuint>(Timestamp_Type::count)> names{};

		names[static_cast<GLuint>(Timestamp_Type::individual_command)] = "individual_command"sv;
		names[static_cast<GLuint>(Timestamp_Type::phase)] = "phase"sv;
		names[static_cast<GLuint>(Timestamp_Type::tick)] = "tick"sv;
		names[static_cast<GLuint>(Timestamp_Type::draw_source)] = "draw_source"sv;
		names[static_cast<GLuint>(Timestamp_Type::illuminate_source)] = "illuminate_source"sv;
		names[static_cast<GLuint>(Timestamp_Type::draw)] = "draw"sv;
		names[static_cast<GLuint>(Timestamp_Type::frame)] = "frame"sv;

		return names;
	}

	constexpr std::array<std::string_view, static_cast<GLuint>(Timestamp_Type::count)> timestamp_type_names{ initialize_timestamp_type_names() };

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
