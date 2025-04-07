#include "game_logic/util/camera/local_world_vector_to_world_vector.h"

namespace game_logic::util::camera
{
	void local_world_vector_to_world_vector
	(
		game_environment::Environment const& environment,
		GLfloat const local_world_x, GLfloat const local_world_y,
		GLint* world_vector_x, GLint* world_vector_y
	)
	{
		// TODO: Add and use functions for fetching view rotation columns
		*world_vector_x = static_cast<GLint>
		(
			local_world_x * environment.state.camera.view_rotation.column_0[0] +
			local_world_y * environment.state.camera.view_rotation.column_0[1]
		);
		*world_vector_y = static_cast<GLint>
		(
			local_world_x * environment.state.camera.view_rotation.column_1[0] +
			local_world_y * environment.state.camera.view_rotation.column_1[1]
		);
	}
}
