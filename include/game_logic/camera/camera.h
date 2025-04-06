#pragma once
#include "util/math/vector_2D.h"

namespace game_logic::camera
{
	struct View_Rotation
	{
		GLfloat column_0[2];
		GLfloat column_1[2];
	};

	struct Camera
	{
		util::math::Vector_2D xy;
		GLint angle;
		GLfloat z;
		View_Rotation view_rotation;
	};
}
