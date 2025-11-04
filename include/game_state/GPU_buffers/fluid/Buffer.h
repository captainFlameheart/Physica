#pragma once
#include "positions/include.h"
#include "velocities/include.h"
#include "bounding_boxes/include.h"

namespace game_state::GPU_buffers::fluid
{
	struct Buffer
	{
		positions::Buffer positions;
		velocities::Buffer velocities;
		bounding_boxes::Buffer bounding_boxes;
	};
}
