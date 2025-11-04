#pragma once
#include "position/include.h"
#include "constrained_point/include.h"

namespace game_state::GPU_buffers::cursor
{
	struct Buffer
	{
		position::Buffer position;
		constrained_point::Buffer constrained_point;
	};
}
