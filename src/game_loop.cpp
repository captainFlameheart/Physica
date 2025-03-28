#include <glad/glad.h>
#include "game_loop.h"
#include "game/game.h"
#include "full_game_state.h"
#include "macros.h"

static void on_glfw_key_event(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	on_key_event(
		get_full_game_state(window),
		key, 
		scancode, 
		action, 
		mods
	);
}

static void on_glfw_cursor_event(GLFWwindow *window, double xpos, double ypos)
{
	on_cursor_event(
		get_full_game_state(window),
		xpos,
		ypos
	);
}

static void on_glfw_mouse_button_event(GLFWwindow* window, int button, int action, int mods)
{
	on_mouse_button_event(
		get_full_game_state(window),
		button,
		action,
		mods
	);
}

static void on_glfw_scroll_event(GLFWwindow* window, double xoffset, double yoffset)
{
	on_scroll_event(
		get_full_game_state(window),
		xoffset,
		yoffset
	);
}

void run_game_loop(GLFWwindow *window)
{
	Full_Game_State game_state;
	glfwSetWindowUserPointer(window, &game_state);

	glfwSetKeyCallback(window, on_glfw_key_event);
	glfwSetCursorPosCallback(window, on_glfw_cursor_event);
	glfwSetMouseButtonCallback(window, on_glfw_mouse_button_event);
	glfwSetScrollCallback(window, on_glfw_scroll_event);

	game_state.window = window;
	game_state.lag = 0;
	initialize_game_state(game_state);

	glfwSetTime(0.0);
	
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
	render(game_state);

	glfwPollEvents();
	while (!glfwWindowShouldClose(window))
	{
		game_state.ticks_this_frame = 0u;
		while 
		(
			(game_state.lag = glfwGetTime() * game_SECOND(game_state)) >= game_TICK(game_state) && 
			game_state.ticks_this_frame < game_MAX_TICKS_PER_FRAME(game_state)
		)
		{
			tick(game_state);
			glfwSetTime(glfwGetTime() - game_TICK(game_state) / game_SECOND(game_state));
			++game_state.ticks_this_frame;
		}

		if (game_state.lag >= game_TICK(game_state))
		{
			glfwSetTime(0.0);
			game_state.lag = 0.0;
			DEBUG_LOG("Game loop tick limit hit!");
		}

		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);
		render(game_state);
		glfwSwapBuffers(window);

		glfwPollEvents();
	}

	free_game_state(game_state);
	glfwSetWindowUserPointer(window, nullptr);
}
