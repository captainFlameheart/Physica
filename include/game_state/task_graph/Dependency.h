#pragma once

namespace game_state::task_graph
{
	template<typename Global_State>
	struct Dependency
	{
		Task& task;
		GLbitfield barriers{ 0u };
	};
}
