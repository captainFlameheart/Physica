#pragma once
#include "Task.h"
#include <vector>

namespace game_state::task_graph
{
	template<typename Global_State>
	struct Task_Graph
	{
		std::vector<Task> tasks;
	};
}
