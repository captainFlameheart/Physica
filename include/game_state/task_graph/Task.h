#pragma once
#include "game_environment/environment.h"
#include "game_state/task_graph/Task_Graph.h"
#include "game_state/task_graph/Dependency.h"
#include <vector>

namespace game_state::task_graph
{
	template<typename Global_State>
	struct Task
	{
		std::vector<Dependency> dependencies;
		void (*run)(::game_environment::Environment& environment, Global_State& state);
	};
}
