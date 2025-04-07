#include "game_logic/util/camera/normalized_x_to_unit_z_x.h"
#include "game_logic/util/projection/INVERSE_SCALE_X.h"

namespace game_logic::util::camera
{
	void normalized_x_to_unit_z_x
	(
		game_environment::Environment const& environment,
		GLfloat normalized_x,
		GLfloat* unit_z_x
	)
	{
		*unit_z_x =
		(
			normalized_x * game_logic__util__projection_INVERSE_SCALE_X(environment)
		);
	}
}
