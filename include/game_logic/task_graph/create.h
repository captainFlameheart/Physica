#pragma once
#include "glad_glfw.h"
#include "game_state/task_graph/include.h"

namespace game_logic::task_graph
{
	template<typename Global_State>
	void do_nothing(game_environment::Environment& environment, Global_State& global_state)
	{
	}

	template<typename Global_State>
	Task_Graph<Global_State> create()
	{
		Task_Graph<Global_State> task_graph;
		task_graph.root_task.run = do_nothing;
		task_graph.root_task.incoming_dependencies = nullptr;
		task_graph.root_task.outgoing_dependencies = nullptr;
		task_graph.root_task.next = nullptr;
		return task_graph;
	}
}
