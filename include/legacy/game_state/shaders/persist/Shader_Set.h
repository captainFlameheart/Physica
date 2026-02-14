#pragma once

namespace legacy::game_state::shaders::persist
{
	struct Shader_Set
	{
		GLuint persist_fluid_contacts_shader;
		GLuint persist_fluid_triangle_contacts_shader;
		GLuint old_triangle_contact_update_shader;
		GLuint update_distance_constraints_shader;
	};
}
