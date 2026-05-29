#pragma once
#include "Task.h"
#include <vector>
#include <string>

namespace game_state::task_graph
{
	template<typename Global_State>
	struct Task_Graph
	{
		bool is_logged;
		std::string name;

		Task root_task;
	};
}
