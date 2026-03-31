#include "glad_glfw.h"
#include "game_environment/environment.h"
#include "game_state/initialize/compile_shaders/environment/include.h"
#include "util/shader/shader.h"
#include "game_logic/shader_util/include.h"

namespace game_logic::initialize::compile_shaders
{
	void compile_send_bounding_volume_hierarchy_to_CPU
	(
		game_environment::Environment& environment, ::game_state::initialize::compile_shaders::environment::Environment compile_environment
	)
	{
		::util::shader::set_shader_statically
		(
			compile_environment.shader_group.compute_shader,
			compile_environment.writable_prefix_source,
			::util::shader::file_to_string("blocks/shader_storage/Bounding_Volume_Hierarchy"),
			::util::shader::file_to_string("tick/bounding_volume_hierarchy/send_to_CPU.comp")
		);

		environment.state.shaders[static_cast<GLuint>(::game_state::shader_indices::tick::process_entities::bounding_volume_hierarchy::send_to_CPU::Indices::send_to_CPU)] = ::util::shader::create_program
		(
			compile_environment.shader_group.compute_shader
		);
	}
}
