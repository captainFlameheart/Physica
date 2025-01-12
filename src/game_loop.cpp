#include <glad/glad.h>
#include "game_loop.h"
#include "renderer.h"
#include "ticker.h"
#include "game_state.h"

void run_game_loop(GLFWwindow* window)
{
	double const tick_delta_time{ 1.0 / 90.0 };

	double ticked_time{ 0.0 };
	Game_State game_state;
	initialize_game_state(game_state);

	int width, height;

	glfwSetTime(ticked_time);
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
	render(ticked_time, game_state, window, 0.0);

	glfwPollEvents();
	while (!glfwWindowShouldClose(window))
	{
		double time_now{ glfwGetTime() };
		double lag;
		while ((lag = time_now - ticked_time) >= tick_delta_time)
		{
			tick(ticked_time, game_state, tick_delta_time);
			ticked_time += tick_delta_time;
		}

		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);
		render(ticked_time, game_state, window, lag);
		glfwSwapBuffers(window);

		glfwPollEvents();
	}

	free_game_state(game_state);
}
