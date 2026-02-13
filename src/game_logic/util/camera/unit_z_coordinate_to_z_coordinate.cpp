#include "RUN_LEGACY.h"
#if RUN_LEGACY == 1
#include "game_logic/util/camera/unit_z_coordinate_to_z_coordinate.h"

namespace game_logic::util::camera
{
	void unit_z_coordinate_to_z_coordinate
	(
		GLfloat const unit_z_coordinate, GLfloat const z,
		GLfloat* camera_local_coordinate
	)
	{
		*camera_local_coordinate = unit_z_coordinate * z;
	}
}
#endif
