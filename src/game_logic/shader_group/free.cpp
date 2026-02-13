#include "game_logic/shader_group/include.h"
#include "game_state/shader_group/include.h"
#include "util/shader/shader.h"

namespace game_logic::shader_group
{
	void free(::game_state::shader_group::Shader_Group& shader_group)
	{
		::util::shader::delete_shader(shader_group.vertex_shader);
		::util::shader::delete_shader(shader_group.fragment_shader);
		::util::shader::delete_shader(shader_group.compute_shader);
	}
}
