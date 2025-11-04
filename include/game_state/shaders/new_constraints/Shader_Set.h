#pragma once

namespace game_state::shaders::new_constraints
{
	struct Shader_Set
	{
		GLuint new_fluid_contacts_shader;
		GLuint new_fluid_triangle_contacts_shader;
		GLuint new_fluid_triangle_contacts_shader_persistent_count_uniform_location;
		GLint new_fluid_contacts_shader_persistent_count_uniform_location;
		GLuint new_triangle_contact_shader;
	};
}
