#include "glad_glfw.h"
#include "game_environment/environment.h"
#include "game_state/initialize/compile_shaders/environment/include.h"
#include "util/shader/shader.h"
#include "game_logic/shader_util/include.h"

namespace game_logic::initialize::compile_shaders
{
	void compile_on_events_polled
	(
		game_environment::Environment& environment, ::game_state::initialize::compile_shaders::environment::Environment compile_environment
	)
	{
		::util::shader::set_shader_statically
		(
			compile_environment.shader_group.compute_shader,
			compile_environment.writable_prefix_source,
			::util::shader::file_to_string("blocks/uniform/On_Events_Polled_Data"),
			::util::shader::file_to_string("on_events_polled/on_events_polled.comp")
		);

		environment.state.shaders[static_cast<GLuint>(::game_state::shader_indices::on_events_polled::Indices::on_events_polled)] = ::util::shader::create_program
		(
			compile_environment.shader_group.compute_shader
		);
	}
}
