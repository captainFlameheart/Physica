#pragma once
#include "glad_glfw.h"
#include "game_environment/environment.h"
#include "game_state/initialize/compile_shaders/environment/include.h"
#include "util/shader/shader.h"
#include "game_logic/shader_util/include.h"

namespace game_logic::initialize::compile_shaders
{
	void compile_draw_point_mass_distance_constraints
	(
		game_environment::Environment& environment, ::game_state::initialize::compile_shaders::environment::Environment compile_environment
	)
	{
		::util::shader::set_shader_statically
		(
			compile_environment.shader_group.vertex_shader,
			compile_environment.readonly_prefix_source,
			::util::shader::file_to_string("draw/constraints/point_mass_distance_constraints/default/draw.vert")
		);

		::util::shader::set_shader_statically
		(
			compile_environment.shader_group.fragment_shader,
			compile_environment.readonly_prefix_source,
			::util::shader::file_to_string("draw/constraints/point_mass_distance_constraints/default/draw.frag")
		);

		environment.state.shaders[static_cast<GLuint>(::game_state::shader_indices::draw::entities::constraints::Indices::point_mass_distance_constraints)] = ::util::shader::create_program
		(
			compile_environment.shader_group.vertex_shader, compile_environment.shader_group.fragment_shader
		);
	}
}
