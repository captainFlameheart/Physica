#include "game_logic/util/camera/world_offset_to_world_position.h"

namespace game_logic::util::camera
{
	void world_offset_to_world_position
	(
		game_environment::Environment const& environment,
		GLint const world_offset_x, GLint const world_offset_y,
		GLint* world_position_x, GLint* world_position_y
	)
	{
		// TODO: Implement and use Vector_2D add function for this
		// TODO: Implement and use const getter for camera x and y
		*world_position_x = environment.state.camera.xy.x + world_offset_x;
		*world_position_y = environment.state.camera.xy.y + world_offset_y;
	}
}
