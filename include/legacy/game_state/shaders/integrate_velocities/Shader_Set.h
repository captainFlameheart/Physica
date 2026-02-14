#pragma once

namespace legacy::game_state::shaders::integrate_velocities
{
	struct Shader_Set
	{
		GLuint rigid_body_velocity_integration_shader;
		GLuint integrate_fluid_velocity_shader;
		GLuint triangle_bounding_box_update_shader;
	};
}
