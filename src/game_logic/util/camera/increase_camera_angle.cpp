#include "game_logic/util/camera/increase_camera_angle.h"
#include "game_logic/util/spatial/TO_RADIANS.h"

namespace game_logic::util::camera
{
	void increase_camera_angle
	(
		game_environment::Environment& environment,
		GLint const angle_increase
	)
	{
		// TODO: Create and use camera_angle(environment) function
		environment.state.camera.angle += angle_increase;
		// TODO: Make sure to not loose precision due to large angles
		GLfloat const radians{ game_logic__util__spatial_TO_RADIANS(environment, environment.state.camera.angle) };
		GLfloat const right_x{ cos(radians) };
		GLfloat const right_y{ sin(radians) };
		environment.state.camera.view_rotation.column_0[0] = right_x;
		environment.state.camera.view_rotation.column_0[1] = -right_y;
		environment.state.camera.view_rotation.column_1[0] = right_y;
		environment.state.camera.view_rotation.column_1[1] = right_x;
	}
}
