#pragma once
#include "glad_glfw.h"
#include "game_state/task_graph/include.h"

namespace game_logic::task_graph
{
	template<typename Global_State>
	Task_Graph<Global_State> create()
	{
		Task_Graph<Global_State> task_graph{};
		return task_graph;
	}
}
