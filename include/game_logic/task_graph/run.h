#pragma once
#include "game_state/task_graph/include.h"
#include <iostream>
#include <vector>

namespace game_logic::task_graph
{
	template<typename Global_State>
	struct Readback_Task
	{
		Task<Global_State>* task;
		GLsync fence;

		GLuint approximated_readback_start_time;

		Readback_Task<Global_State>* previous;
		Readback_Task<Global_State>* next;
	};

	template<typename Global_State>
	void remove_incoming_dependency
	(
		::game_environment::Environment& environment, Global_State& global_state,
		::game_state::task_graph::Task_Graph<Global_State>& task_graph,
		GLbitfield barriers,
		Task*& current_task, Dependency*& finishable_dependencies, GLbitfield& next_barriers, Task*& next_tasks,
		Dependency* dependency
	)
	{
		if (dependency->previous_incoming == nullptr)
		{
			dependency->task_1->incoming_dependencies = dependency->next;
		}
		else
		{
			dependency->task_1->incoming_dependencies = 
		}

		Dependency** referencing_next_pointer;

			dependency->task_1
		delete dependency;
	}

	template<typename Global_State>
	void on_barriers_placed
	(
		::game_environment::Environment& environment, Global_State& global_state,
		::game_state::task_graph::Task_Graph<Global_State>& task_graph,
		GLbitfield barriers,
		Task*& current_task, Dependency*& finishable_dependencies, GLbitfield& next_barriers, Task*& next_tasks
	)
	{
		GLbitfield mask{ ~barriers };

		Dependency** finishable_dependency_pointer = &finishable_dependencies;
		while (finishable_dependency != nullptr)
		{
			Dependency* finishable_dependency = *finishable_dependency_pointer;
			finishable_dependency->barriers &= mask;
			if (finishable_dependency->barriers == 0u)
			{
				*finishable_dependency_pointer = finishable_dependency->next;
				remove_incoming_dependency(finishable_dependency);
			}
			else
			{
				finishable_dependency_pointer = &finishable_dedpendency->next;
			}
		}
	}

	template<typename Global_Stat>
	void on_preceeding_task_completed(
		::game_environment::Environment& environment, Global_State& global_state,
		::game_state::task_graph::Task_Graph<Global_State>& task_graph,
		Dependency* dependency, Task* task,
		Task*& current_task, Dependency*& finishable_dependencies, GLbitfield& next_barriers, Task*& next_tasks
	)
	{
		dependency->task_0_completed = true;
		dependency->next_finishable = finishable_dependencies;
		finishable_dependencies = dependency->next_finishable;

		GLbitfield incoming_barriers{ 0u };
		Dependency* incoming_dependency{ task->incoming_dependencies };
		while (incoming_dependency != nullptr)
		{
			if (!incoming_dependency->task_0_completed)
			{
				return;
			}
			incoming_barriers |= incoming_dependency->barrier;
		}
		if (incoming_barriers == 0u)
		{
			task->next = current_task;
			current_task = task;
		}
		else
		{
			next_barriers |= incoming_barriers;
			task->next = next_tasks;
			next_tasks = task;
		}
	}
	
	template<typename Global_State>
	void on_task_completed(
		::game_environment::Environment& environment, Global_State& global_state,
		::game_state::task_graph::Task_Graph<Global_State>& task_graph,
		Task* completed_task,
		Task*& current_task, Dependency*& finishable_dependencies, GLbitfield& next_barriers, Task*& next_tasks
	)
	{
		while (completed_task->outgoing_dependencies != nullptr)
		{
			Dependency* next{ completed_task->outgoing_dependencies };

			on_preceeding_task_completed
			(
				environment, global_state,
				task_graph,
				completed_task->outgoing_dependencies->task_1,
				current_task, finishable_dependencies, next_barriers, next_tasks
			);

			completed_task->outgoing_dependencies = next;
		}

		delete completed_task;
	}

	template<typename Global_State>
	Task* run
	(
		::game_environment::Environment& environment, Global_State& global_state,
		::game_state::task_graph::Task_Graph<Global_State>& task_graph
	)
	{
		GLuint approximated_command_issue_time{ 1u };	// TODO: Move out to environment.

		std::cout << "Run task graph \"" << task_graph.name << "\":\n";

		Task* current_task = &task_graph.root_task;
		Dependency* finishable_dependencies = nullptr;
		//GLbitfield next_barriers = 0u;
		//Task* next_tasks = nullptr;

		Readback_Task* oldest_readback_task = nullptr;
		Readback_Task* newest_readback_task = nullptr;
		
		GLuint approximated_issued_tasks_end_time{ 0u };

		do
		{
			if (task_graph.is_logged)
			{
				std::cout << "	Run task \"" << current_task << "\"\n";
			}
			current_task->run(current_task, environment, global_state);
			approximated_issued_tasks_end_time += current_task->approximated_execution_time;

			

			if (current_task->starts_readback)
			{
				glMemoryBarrier(GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT);
				GLsync fence{ glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0u) };


			}

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

			Readback_Task* readback_task = oldest_readback_task;
			while (
				readback_task != nullptr
			)
			{
				GLuint approximated_readback_time{ readback_task->task->approximate_readback_time() };
				GLuint approximated_ping_pong_end_time{
					readback_task->approximated_readback_start_time + approximated_readback_time + approximated_command_issue_time
				};
				if (current_task == nullptr || approximated_issued_tasks_end_time > approximated_ping_pong_end_time)
				{
					GLenum fence_status{ glClientWaitSync(readback_task->fence, GL_SYNC_FLUSH_COMMANDS_BIT, 0u) };
					while (fence_status != GL_ALREADY_SIGNALED && fence_status != GL_CONDITION_SATISFIED)
					{
						if (fence_status == GL_WAIT_FAILED)
						{
							std::cerr << "Fence wait failed!" << std::endl;
						}
						if (fence_status == GL_TIMEOUT_EXPIRED)
						{
							// TODO: Run unit of CPU work.
						}

						fence_status = glClientWaitSync(readback_task->fence, 0u, 0u);
					}
					glDeleteSync(readback_task->fence);

					// TODO: Delete readback task from list.
				}

				readback_task = readback_task->next;
			}

			if (current_task == nullptr)
			{
				if (task_graph.is_logged)
				{
					std::cout << "\n	Place memory barrier " << next_barriers << "\n";
				}
				if (next_barriers != 0u)
				{
					glMemoryBarrier(next_barriers);
					on_barriers_placed(next_barriers);
				}
				current_task = next_tasks;
				next_barriers = 0u;
				next_tasks = nullptr;
			}
		} while (current_task != nullptr);
	}
}
