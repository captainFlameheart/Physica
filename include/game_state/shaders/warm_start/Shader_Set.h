#pragma once

namespace game_state::shaders::warm_start
{
	struct Shader_Set
	{
		GLuint warm_start_fluid_contacts_shader;
		GLuint warm_start_fluid_triangle_contacts_shader;
		GLuint warm_start_contact_impulses_shader;
		GLuint warm_start_distance_constraints_shader;
		GLuint update_and_warm_start_cursor_constraint_shader;
	};
}
