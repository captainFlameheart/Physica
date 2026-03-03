#pragma once
#include "GPU_only/include.h"
#include "uniform/include.h"

namespace game_state::buffers
{
	struct State
	{
		GPU_only::State GPU_only;
		uniform::State uniform;
	};
}
