#pragma once
#include "glad_glfw.h"
#include "game_environment/environment.h"
#include "game_state/initialize/compile_shaders/environment/include.h"
#include "util/shader/shader.h"
#include "game_logic/shader_util/include.h"

namespace game_logic::initialize::compile_shaders
{
	void compile_clear_invalid_rigid_body_circle_contacts
	(
		game_environment::Environment& environment, ::game_state::initialize::compile_shaders::environment::Environment compile_environment
	)
	{
		::util::shader::set_shader_statically
		(
			compile_environment.shader_group.compute_shader,
			compile_environment.writable_prefix_source,
			::util::shader::file_to_string("reusable/initialize_contacts/clear_invalid_contacts/rigid_body_circle/clear_invalid_rigid_body_circle_contacts.comp")
		);

		environment.state.shaders[static_cast<GLuint>(::game_state::shader_indices::reusable::initialize_contacts::clear_invalid_contacts::Indices::rigid_body_circle)] = ::util::shader::create_program
		(
			compile_environment.shader_group.compute_shader
		);
	}
}
