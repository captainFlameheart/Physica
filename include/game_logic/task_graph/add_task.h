#pragma once
#include "game_state/task_graph/include.h"

namespace game_logic::task_graph
{
	template<typename Global_State>
	void add_task(::game_state::Task_Graph<Global_State>& task_graph, ::game_state::Task<Global_State> const& task)
	{
		task_graph.tasks.push_back(task);
	}
}
