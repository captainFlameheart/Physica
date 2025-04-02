#pragma once
#include "util/math/vector_2D.h"

namespace game::camera
{
	struct Transform
	{
		util::math::Vector_2D xy;
		GLint z;
		GLint angle;
	};

	struct View_Rotation
	{
		GLfloat column_0[2];
		GLfloat column_1[2];
	};

	struct Camera
	{
		Transform transform;
		View_Rotation view_rotation;
	};
}
