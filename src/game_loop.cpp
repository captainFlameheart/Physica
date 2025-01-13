#include <glad/glad.h>
#include "game_loop.h"
#include "game.h"
#include "full_game_state.h"

static void on_glfw_key_event(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	Full_Game_State &full_game_state{ get_full_game_state(window) };
	on_key_event(
		window, 
		full_game_state.tick_delta_time, 
		full_game_state.max_ticks_per_frame,
		full_game_state.game_state, 
		full_game_state.processed_time, 
		key, 
		scancode, 
		action, 
		mods
	);
}

static void on_glfw_cursor_event(GLFWwindow *window, double xpos, double ypos)
{
	Full_Game_State &full_game_state{ get_full_game_state(window) };
	on_cursor_event(
		window,
		full_game_state.tick_delta_time,
		full_game_state.max_ticks_per_frame,
		full_game_state.game_state,
		full_game_state.processed_time,
		xpos,
		ypos
	);
}

static void on_glfw_mouse_button_event(GLFWwindow* window, int button, int action, int mods)
{
	Full_Game_State& full_game_state{ get_full_game_state(window) };
	on_mouse_button_event(
		window,
		full_game_state.tick_delta_time,
		full_game_state.max_ticks_per_frame,
		full_game_state.game_state,
		full_game_state.processed_time,
		button,
		action,
		mods
	);
}

static void on_glfw_scroll_event(GLFWwindow* window, double xoffset, double yoffset)
{
	Full_Game_State& full_game_state{ get_full_game_state(window) };
	on_scroll_event(
		window,
		full_game_state.tick_delta_time,
		full_game_state.max_ticks_per_frame,
		full_game_state.game_state,
		full_game_state.processed_time,
		xoffset,
		yoffset
	);
}

void run_game_loop(GLFWwindow* window)
{
	double tick_delta_time{ 1.0 / 90.0 };
	unsigned max_ticks_per_frame{ 10 };
	Game_State game_state;
	double processed_time{ 0.0 };

	glfwSetKeyCallback(window, on_glfw_key_event);
	glfwSetCursorPosCallback(window, on_glfw_cursor_event);
	glfwSetMouseButtonCallback(window, on_glfw_mouse_button_event);
	glfwSetScrollCallback(window, on_glfw_scroll_event);

	Full_Game_State full_game_state{tick_delta_time, max_ticks_per_frame, game_state, processed_time};
	glfwSetWindowUserPointer(window, &full_game_state);

	initialize_game_state(window, tick_delta_time, max_ticks_per_frame, game_state);

	glfwSetTime(processed_time);
	
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
	render(window, tick_delta_time, max_ticks_per_frame, game_state, processed_time, 0.0);

	glfwPollEvents();
	while (!glfwWindowShouldClose(window))
	{
		double time_now{ glfwGetTime() };
		double lag;
		unsigned ticks{ 0 };
		while ((lag = time_now - processed_time) >= tick_delta_time && ticks < max_ticks_per_frame)
		{
			tick(window, tick_delta_time, max_ticks_per_frame, game_state, processed_time, ticks);
			processed_time += tick_delta_time;
			++ticks;
		}

		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);
		render(window, tick_delta_time, max_ticks_per_frame, game_state, processed_time, 0.0);
		glfwSwapBuffers(window);

		glfwPollEvents();
	}

	free_game_state(window, tick_delta_time, max_ticks_per_frame, game_state, processed_time);
	glfwSetWindowUserPointer(window, nullptr);
}
