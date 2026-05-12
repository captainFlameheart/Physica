#pragma once
#include "glad_glfw.h"
#include <string>
#include <string_view>
#include <array>
using namespace std::string_view_literals;

namespace game_state::profiling
{
	enum struct Timing_Type : GLuint
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

	constexpr std::array<std::string_view, static_cast<GLuint>(Timing_Type::count)> initialize_timing_type_names()
	{
		std::array<std::string_view, static_cast<GLuint>(Timing_Type::count)> names{};

		names[static_cast<GLuint>(Timing_Type::individual_command)] = "individual_command"sv;
		names[static_cast<GLuint>(Timing_Type::phase)] = "phase"sv;
		names[static_cast<GLuint>(Timing_Type::tick)] = "tick"sv;
		names[static_cast<GLuint>(Timing_Type::draw_source)] = "draw_source"sv;
		names[static_cast<GLuint>(Timing_Type::illuminate_source)] = "illuminate_source"sv;
		names[static_cast<GLuint>(Timing_Type::draw)] = "draw"sv;
		names[static_cast<GLuint>(Timing_Type::frame)] = "frame"sv;

		return names;
	}

	constexpr std::array<std::string_view, static_cast<GLuint>(Timing_Type::count)> timing_type_names{ initialize_timing_type_names() };
}
