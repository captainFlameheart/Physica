#include "RUN_LEGACY.h"
#if RUN_LEGACY == 1
#include "game_logic/util/camera/unit_z_vector_to_z_vector.h"
#include "game_logic/util/camera/unit_z_x_to_z_x.h"
#include "game_logic/util/camera/unit_z_y_to_z_y.h"

namespace game_logic::util::camera
{
	void unit_z_vector_to_z_vector
	(
		GLfloat const unit_z_x, GLfloat const unit_z_y,
		GLfloat const z,
		GLfloat* z_x, GLfloat* z_y
	)
	{
		unit_z_x_to_z_x(unit_z_x, z, z_x);
		unit_z_y_to_z_y(unit_z_y, z, z_y);
	}
}
#endif
