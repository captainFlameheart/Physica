#include "game_logic/shader_group/include.h"
#include "game_state/shader_group/include.h"
#include "util/shader/shader.h"

namespace game_logic::shader_group
{
	void initialize(::game_state::shader_group::Shader_Group& shader_group)
	{
		shader_group.vertex_shader = ::util::shader::create_shader(GL_VERTEX_SHADER);
		shader_group.fragment_shader = ::util::shader::create_shader(GL_FRAGMENT_SHADER);
		shader_group.compute_shader = ::util::shader::create_shader(GL_COMPUTE_SHADER);
	}
}
