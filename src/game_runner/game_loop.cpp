#include <glad/glad.h>
#include "game_runner/game_loop.h"
#include "game_logic/game.h"
#include "macros/macros.h"

namespace game_runner
{
	static void on_glfw_key_event(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		game::on_key_event(
			game_environment::from(window),
			key, 
			scancode, 
			action, 
			mods
		);
	}

	static void on_glfw_cursor_event(GLFWwindow *window, double xpos, double ypos)
	{
		game::on_cursor_event(
			game_environment::from(window),
			xpos,
			ypos
		);
	}

	static void on_glfw_mouse_button_event(GLFWwindow* window, int button, int action, int mods)
	{
		game::on_mouse_button_event(
			game_environment::from(window),
			button,
			action,
			mods
		);
	}

	static void on_glfw_scroll_event(GLFWwindow* window, double xoffset, double yoffset)
	{
		game::on_scroll_event(
			game_environment::from(window),
			xoffset,
			yoffset
		);
	}

	void run_game_loop(GLFWwindow *window)
	{
		game_environment::Environment game_environment;
		glfwSetWindowUserPointer(window, &game_environment);

		glfwSetKeyCallback(window, on_glfw_key_event);
		glfwSetCursorPosCallback(window, on_glfw_cursor_event);
		glfwSetMouseButtonCallback(window, on_glfw_mouse_button_event);
		glfwSetScrollCallback(window, on_glfw_scroll_event);

		game_environment.window = window;
		game_environment.lag = 0;
		game::initialize(game_environment);

		glfwSetTime(0.0);
	
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);
		game::render(game_environment);

		glfwPollEvents();
		while (!glfwWindowShouldClose(window))
		{
			game_environment.ticks_this_frame = 0u;
			while 
			(
				(game_environment.lag = glfwGetTime() * game_SECOND(game_environment)) >= game_TICK(game_environment) &&
				game_environment.ticks_this_frame < game_MAX_TICKS_PER_FRAME(game_environment)
			)
			{
				game::tick(game_environment);
				glfwSetTime(glfwGetTime() - game_TICK(game_environment) / game_SECOND(game_environment));
				++game_environment.ticks_this_frame;
			}

			if (game_environment.lag >= game_TICK(game_environment))
			{
				glfwSetTime(0.0);
				game_environment.lag = 0;
				DEBUG_LOG("Game loop tick limit hit!");
			}

			glfwGetFramebufferSize(window, &width, &height);
			glViewport(0, 0, width, height);
			game::render(game_environment);
			glfwSwapBuffers(window);

			glfwPollEvents();
		}

		game::free(game_environment);
		glfwSetWindowUserPointer(window, nullptr);
	}
}
