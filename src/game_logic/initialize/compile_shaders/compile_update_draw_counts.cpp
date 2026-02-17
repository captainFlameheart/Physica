#pragma once
#include "glad_glfw.h"
#include "game_environment/environment.h"
#include "game_state/initialize/compile_shaders/environment/include.h"
#include "util/shader/shader.h"
#include "game_logic/shader_util/include.h"

namespace game_logic::initialize::compile_shaders
{
	void compile_update_draw_counts
	(
		game_environment::Environment& environment, ::game_state::initialize::compile_shaders::environment::Environment compile_environment
	)
	{
		::util::shader::set_shader_statically
		(
			compile_environment.shader_group.compute_shader,
			compile_environment.writable_prefix_source,
			::util::shader::file_to_string("draw/update_counts.comp")
		);

		::game_logic::shader_util::print_source(compile_environment.shader_group.compute_shader);

		environment.state.shaders[static_cast<GLuint>(::game_state::shader_indices::draw::update_counts::Indices::update_counts)] = ::util::shader::create_program
		(
			compile_environment.shader_group.compute_shader
		);
	}
}
