#pragma once
#include "game_state/task_graph/include.h"
#include <iostream>

namespace game_logic::task_graph
{
	template<typename Global_State>
	Task* run
	(
		::game_environment::Environment& environment, Global_State& global_state,
		::game_state::task_graph::Task_Graph<Global_State>& task_graph
	)
	{
		std::cout << "Run task graph \"" << task_graph.name << "\":\n";

		Task* current_task = &task_graph.root_task;
		GLbitfield next_barriers = 0u;
		Task* next_tasks = nullptr;
		do
		{
			if (task_graph.is_logged)
			{
				std::cout << "	Run task \"" << current_task << "\"\n";
			}
			current_task->run(current_task, environment, global_state);
			
			while (current_task->outgoing_dependencies != nullptr)
			{
				next_barriers |= current_task->outgoing_dependencies->barriers;

				current_task->outgoing_dependencies->task_1->next = next_tasks;
				next_tasks = current_task->outgoing_dependencies->task_1;

				current_task->outgoing_dependencies = current_task->outgoing_dependencies->next_outgoing;
			}

			Task* next = current_task->next;
			delete current_task;
			current_task = next;

			if (current_task == nullptr)
			{
				if (task_graph.is_logged)
				{
					std::cout << "\n	Place memory barrier " << next_barriers << "\n";
				}
				if (next_barriers != 0u)
				{
					glMemoryBarrier(next_barriers);
				}
				current_task = next_tasks;
				next_barriers = 0u;
				next_tasks = nullptr;
			}
		} while (current_task != nullptr);
	}
}
