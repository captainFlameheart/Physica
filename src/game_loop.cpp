#include <glad/glad.h>
#include "game_loop.h"
#include "game.h"

static struct Full_Game_State
{
	double &tick_delta_time;
	unsigned &max_ticks_per_frame;
	Game_State &game_state;
	double &ticked_time;
};

static void on_glfw_key_event(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	Full_Game_State &full_game_state{ *static_cast<Full_Game_State*>(glfwGetWindowUserPointer(window)) };
	on_key_event(
		window, 
		full_game_state.tick_delta_time, 
		full_game_state.max_ticks_per_frame,
		full_game_state.game_state, 
		full_game_state.ticked_time, 
		key, 
		scancode, 
		action, 
		mods
	);
}

void run_game_loop(GLFWwindow* window)
{
	double tick_delta_time;
	unsigned max_ticks_per_frame;
	Game_State game_state;
	initialize_game_state(tick_delta_time, max_ticks_per_frame, game_state);
	double ticked_time{ 0.0 };

	Full_Game_State full_game_state{tick_delta_time, max_ticks_per_frame, game_state, ticked_time};
	glfwSetWindowUserPointer(window, &full_game_state);

	glfwSetKeyCallback(window, on_glfw_key_event);

	glfwSetTime(ticked_time);
	
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
	render(window, tick_delta_time, max_ticks_per_frame, game_state, ticked_time, 0.0);

	glfwPollEvents();
	while (!glfwWindowShouldClose(window))
	{
		double time_now{ glfwGetTime() };
		double lag;
		unsigned ticks{ 0 };
		while ((lag = time_now - ticked_time) >= tick_delta_time && ticks < max_ticks_per_frame)
		{
			tick(window, tick_delta_time, max_ticks_per_frame, game_state, ticked_time, ticks);
			ticked_time += tick_delta_time;
			++ticks;
		}

		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);
		render(window, tick_delta_time, max_ticks_per_frame, game_state, ticked_time, 0.0);
		glfwSwapBuffers(window);

		glfwPollEvents();
	}

	free_game_state(game_state);
}
