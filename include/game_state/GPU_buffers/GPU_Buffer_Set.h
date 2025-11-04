#pragma once
#include "rigid_bodies/include.h"
#include "fluid/include.h"

namespace game_state::GPU_buffers
{
	struct GPU_Buffer_Set
	{
		rigid_bodies::Buffer rigid_bodies;
		fluid::Buffer fluid;
	};
}
