#pragma once

namespace game_state::task_graph
{
	template<typename Global_State>
	struct Dependency
	{
		Task* task_0;
		Task* task_1;
		GLbitfield barriers;
		bool task_0_completed;

		Dependency* previous_incoming;
		Dependency* next_incoming;

		union
		{
			Dependency* previous_outgoing;
		}
		union
		{
			Dependency* next_outgoing;
			Dependency* next_finishable;
		};
	};
}
