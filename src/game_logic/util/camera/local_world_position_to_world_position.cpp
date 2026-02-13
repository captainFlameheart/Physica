#include "RUN_LEGACY.h"
#if RUN_LEGACY == 1
#include "game_logic/util/camera/camera.h"
#include "game_environment/environment.h"
#include "game_logic/util/camera/local_world_vector_to_world_vector.h"
#include "game_logic/util/camera/world_offset_to_world_position.h"

namespace game_logic::util::camera
{
	void local_world_position_to_world_position(
		game_environment::Environment const& environment,
		GLfloat const local_world_x, GLfloat const local_world_y,
		GLint* world_x, GLint* world_y
	)
	{
		local_world_vector_to_world_vector
		(
			environment, local_world_x, local_world_y, world_x, world_y
		);
		world_offset_to_world_position
		(
			environment, *world_x, *world_y, world_x, world_y
		);
	}
}
#endif
