#pragma once
#include "glad_glfw.h"
#include "game_environment/environment.h"
#include "game_state/initialize/compile_shaders/environment/include.h"
#include "util/shader/shader.h"
#include "game_logic/shader_util/include.h"

namespace game_logic::initialize::compile_shaders
{
	void compile_perform_rigid_body_circle_contact_constraint_compaction
	(
		game_environment::Environment& environment, ::game_state::initialize::compile_shaders::environment::Environment compile_environment
	)
	{
		::util::shader::set_shader_statically
		(
			compile_environment.shader_group.compute_shader,
			compile_environment.writable_prefix_source,
			::util::shader::file_to_string("tick/pre_constraints/perform_compaction/perform_rigid_body_circle_contact_constraint_compaction.comp")
		);

		environment.state.shaders[static_cast<GLuint>(::game_state::shader_indices::tick::process_entities::pre_constraints::perform_compaction::Indices::perform_rigid_body_circle_contact_constraint_compaction)] = ::util::shader::create_program
		(
			compile_environment.shader_group.compute_shader
		);
	}
}
