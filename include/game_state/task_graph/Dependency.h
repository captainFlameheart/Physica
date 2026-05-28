#pragma once

namespace game_state::task_graph
{
	template<typename Global_State>
	struct Dependency
	{
		Task* task_0;
		Task* task_1;
		GLbitfield barriers;

		Dependency* previous_incoming;
		Dependency* next_incoming;

		Dependency* previous_outgoing;
		Dependency* next_outgoing;
	};
}
