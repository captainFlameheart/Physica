#pragma once

namespace legacy::game_state::shaders::solve
{
	struct Shader_Set
	{
		GLuint solve_fluid_contacts_shader;
		GLuint solve_fluid_triangle_contacts_shader;
		GLuint solve_contact_velocities_shader;
		GLuint solve_distance_constraints_shader;
		GLuint solve_cursor_constraint_shader;
	};
}
