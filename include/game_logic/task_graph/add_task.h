#pragma once
#include "game_state/task_graph/include.h"

namespace game_logic::task_graph
{
	template<typename Global_State>
	Task* add_task
	(
		::game_state::task_graph::Task_Graph<Global_State>& task_graph,
		void (*run)(::game_environment::Environment& environment, Global_State& state),
		std::initializer_list<::game_state::task_graph::Incoming_Dependency> incoming_dependencies
	)
	{
		Task* task = new Task{};
		task->run = run;
		task->incoming_dependencies = nullptr;
		task->outgoing_dependencies = nullptr;

		for (::game_state::task_graph::Incoming_Dependency& incoming_dependency : incoming_dependencies)
		{
			::game_state::task_graph::Dependency* dependency{ new ::game_state::task_graph::Dependency{} };
			dependency->task_0 = incoming_dependency.task;
			dependency->task_1 = task;
			dependency->barriers = incoming_dependency.barriers;

			{
				dependency->previous_outgoing = nullptr;
				dependency->next_outgoing = incoming_dependency.task->outgoing_dependencies;

				if (incoming_dependency.task->outgoing_dependencies == nullptr)
				{
					incoming_dependency.task->outgoing_dependencies = dependency;
				}
				else
				{
					incoming_dependency.task->outgoing_dependencies->previous_outgoing = dependency;
					incoming_dependency.task->outgoing_dependencies = dependency;
				}
			}

			{
				dependency->previous_incoming = nullptr;
				dependency->next_incoming = task->incoming_dependencies;

				if (task->incoming_dependencies == nullptr)
				{
					task->incoming_dependencies = dependency;
				}
				else
				{
					task->incoming_dependencies->previous_incoming = dependency;
					task->incoming_dependencies = dependency;
				}
			}
		}
		task_graph.tasks.push_back(task);
	}
}
