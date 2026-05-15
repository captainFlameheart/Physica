#pragma once
#include "glad_glfw.h"
#include "game_environment/environment.h"
#include "game_state/initialize/compile_shaders/environment/include.h"
#include "util/shader/shader.h"
#include "game_logic/shader_util/include.h"

namespace game_logic::initialize::compile_shaders
{
	void compile_draw_timing_bar_chart
	(
		game_environment::Environment& environment, ::game_state::initialize::compile_shaders::environment::Environment compile_environment
	)
	{
		std::string blocks
		{
			::util::shader::file_to_string("blocks/uniform/Timing_Configuration") +
			::util::shader::file_to_string("blocks/uniform/Timing_Metadata") +
			::util::shader::file_to_string("blocks/shader_storage/Timestamps") +
			::util::shader::file_to_string("blocks/shader_storage/Timestamp_Metadata")
		};

		::util::shader::set_shader_statically
		(
			compile_environment.shader_group.vertex_shader,
			compile_environment.readonly_prefix_source,
			blocks,
			::util::shader::file_to_string("draw/profiling/timing/bar_chart/draw.vert")
		);

		::util::shader::set_shader_statically
		(
			compile_environment.shader_group.fragment_shader,
			compile_environment.readonly_prefix_source,
			blocks,
			::util::shader::file_to_string("draw/profiling/timing/bar_chart/draw.frag")
		);

		environment.state.shaders[static_cast<GLuint>(::game_state::shader_indices::draw::profiling::Indices::timing_bar_chart)] = ::util::shader::create_program
		(
			compile_environment.shader_group.vertex_shader, compile_environment.shader_group.fragment_shader
		);
	}
}
