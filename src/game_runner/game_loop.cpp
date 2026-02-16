#include <glad/glad.h>
#include "game_runner/game_loop.h"
#include "game_logic/game.h"
//#include "game_logic/util/tick/delta_time/SECONDS.h"
//#include "game_logic/util/tick/MAX_TICKS_PER_FRAME.h"
//#include "macros/macros.h"
#include "game_state/units/include.h"

namespace game_runner
{
	static void on_framebuffer_size_changed(GLFWwindow* window, int width, int height)
	{
		game_logic::_on_framebuffer_size_changed
		(
			game_environment::from(window), 
			width, height
		);
	}

	static void on_glfw_key_event(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		game_logic::_on_key_event(
			game_environment::from(window),
			key, 
			scancode, 
			action, 
			mods
		);
	}

	static void on_glfw_cursor_event(GLFWwindow *window, double xpos, double ypos)
	{
		game_logic::_on_cursor_event(
			game_environment::from(window),
			xpos,
			ypos
		);
	}

	static void on_glfw_mouse_button_event(GLFWwindow* window, int button, int action, int mods)
	{
		game_logic::_on_mouse_button_event(
			game_environment::from(window),
			button,
			action,
			mods
		);
	}

	static void on_glfw_scroll_event(GLFWwindow* window, double xoffset, double yoffset)
	{
		game_logic::_on_scroll_event(
			game_environment::from(window),
			xoffset,
			yoffset
		);
	}

	void run_game_loop(GLFWwindow *window)
	{
		game_environment::Environment game_environment;
		glfwSetWindowUserPointer(window, &game_environment);

		glfwSetFramebufferSizeCallback(window, on_framebuffer_size_changed);
		glfwSetKeyCallback(window, on_glfw_key_event);
		glfwSetCursorPosCallback(window, on_glfw_cursor_event);
		glfwSetMouseButtonCallback(window, on_glfw_mouse_button_event);
		glfwSetScrollCallback(window, on_glfw_scroll_event);

		game_environment.window = window;
		game_environment.lag = 0;
		game_logic::_initialize(game_environment);

		glfwSetTime(0.0);
	
		game_logic::_draw(game_environment);
		glfwSwapBuffers(window);

		glfwPollEvents();

#if RUN_LEGACY == 1 
		while (!glfwWindowShouldClose(window))
		{
			game_environment.ticks_this_frame = 0u;
			while 
			(
				(game_environment.lag = static_cast<GLfloat>(glfwGetTime()) >= game_logic__util__tick__delta_time_SECONDS(environment)) && 
				game_environment.ticks_this_frame < game_MAX_TICKS_PER_FRAME(game_environment)
			)
			{
				game_logic::_tick(game_environment);
				glfwSetTime(glfwGetTime() - static_cast<double>(game_logic__util__tick__delta_time_SECONDS(game_environment)));
				++game_environment.ticks_this_frame;
			}

			if (game_environment.lag >= game_logic__util__tick__delta_time_SECONDS(game_environment))
			{
				glfwSetTime(0.0);
				game_environment.lag = 0;
				DEBUG_LOG("Game loop tick limit hit!");
			}

			game_logic::_draw(game_environment);
			glfwSwapBuffers(window);

			glfwPollEvents();
		}
#else
		constexpr GLuint max_ticks_per_frame{ 100u };
		while (!glfwWindowShouldClose(window))
		{
			game_environment.ticks_this_frame = 0u;
			while
			(
				(game_environment.lag = static_cast<GLfloat>(glfwGetTime()) >= ::game_state::units::time_unit_in_seconds) &&
				game_environment.ticks_this_frame < max_ticks_per_frame
			)
			{
				game_logic::_tick(game_environment);
				glfwSetTime(glfwGetTime() - static_cast<double>(::game_state::units::time_unit_in_seconds));
				++game_environment.ticks_this_frame;
			}

			if (game_environment.lag >= ::game_state::units::time_unit_in_seconds)
			{
				glfwSetTime(0.0);
				game_environment.lag = 0.0f;
				DEBUG_LOG("Game loop tick limit hit!");
			}

			game_logic::_draw(game_environment);
			glfwSwapBuffers(window);

			glfwPollEvents();
		}
#endif

		game_logic::_free(game_environment);
		glfwSetWindowUserPointer(window, nullptr);
	}
}
