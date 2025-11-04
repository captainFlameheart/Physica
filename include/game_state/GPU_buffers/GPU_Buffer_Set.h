#pragma once
#include "rigid_bodies/include.h"
#include "fluid/include.h"
#include "fluid_triangle/include.h"
#include "cursor/include.h"

namespace game_state::GPU_buffers
{
	struct GPU_Buffer_Set
	{
		cursor::Buffer cursor;
		rigid_bodies::Buffer rigid_bodies;
		fluid::Buffer fluid;
		fluid_triangle::Buffer fluid_triangle;
	};
}
