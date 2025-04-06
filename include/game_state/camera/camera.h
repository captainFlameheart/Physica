#pragma once
#include "util/math/vector_2D.h"
#include "game_state/camera/view_rotation.h"

namespace game_state::camera
{
	struct Camera
	{
		util::math::Vector_2D xy;
		GLint angle;
		GLfloat z;
		View_Rotation view_rotation;
	};
}
