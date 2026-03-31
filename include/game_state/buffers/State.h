#pragma once
#include "GPU_only/include.h"
#include "uniform/include.h"
#include "bounding_volume_hierarchy/include.h"

namespace game_state::buffers
{
	struct State
	{
		GPU_only::State GPU_only;
		uniform::State uniform;
		bounding_volume_hierarchy::State bounding_volume_hierarchy;
	};
}
