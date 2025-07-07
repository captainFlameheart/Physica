#include <glad/glad.h>
#include "game_runner/game_loop.h"
#include "game_logic/game.h"
#include "game_logic/util/tick/delta_time/SECONDS.h"
#include "game_logic/util/tick/MAX_TICKS_PER_FRAME.h"
#include "macros/macros.h"

namespace game_runner
{
	static void on_framebuffer_size_changed(GLFWwindow* window, int width, int height)
	{
		game_logic::on_framebuffer_size_changed
		(
			game_environment::from(window), 
			width, height
		);
	}

	static void on_glfw_key_event(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		game_logic::on_key_event(
			game_environment::from(window),
			key, 
			scancode, 
			action, 
			mods
		);
	}

	static void on_glfw_cursor_event(GLFWwindow *window, double xpos, double ypos)
	{
		game_logic::on_cursor_event(
			game_environment::from(window),
			xpos,
			ypos
		);
	}

	static void on_glfw_mouse_button_event(GLFWwindow* window, int button, int action, int mods)
	{
		game_logic::on_mouse_button_event(
			game_environment::from(window),
			button,
			action,
			mods
		);
	}

	static void on_glfw_scroll_event(GLFWwindow* window, double xoffset, double yoffset)
	{
		game_logic::on_scroll_event(
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
		game_logic::initialize(game_environment);

		glfwSetTime(0.0);
	
		game_logic::render(game_environment);
		glfwSwapBuffers(window);

		glfwPollEvents();

		GLuint draw_count{ 0 };
		while (!glfwWindowShouldClose(window))
		{
			game_environment.ticks_this_frame = 0u;
			while 
			(
				(game_environment.lag = static_cast<GLfloat>(glfwGetTime()) >= game_logic__util__tick__delta_time_SECONDS(environment)) && 
				game_environment.ticks_this_frame < game_MAX_TICKS_PER_FRAME(game_environment)
			)
			{
				game_logic::tick(game_environment);
				glfwSetTime(glfwGetTime() - static_cast<double>(game_logic__util__tick__delta_time_SECONDS(game_environment)));
				++game_environment.ticks_this_frame;
			}

			if (game_environment.lag >= game_logic__util__tick__delta_time_SECONDS(game_environment))
			{
				glfwSetTime(0.0);
				game_environment.lag = 0;
				DEBUG_LOG("Game loop tick limit hit!");
			}

			game_logic::render(game_environment);
			glfwSwapBuffers(window);

			if (draw_count % 120u == 0u)
			{
				GLint time_elapsed_query_done;
				glGetQueryObjectiv(game_environment.state.time_elapsed_query, GL_QUERY_RESULT_AVAILABLE, &time_elapsed_query_done);
				if (time_elapsed_query_done == GL_TRUE)
				{
					GLuint64 time_elapsed;
					glGetQueryObjectui64v(game_environment.state.time_elapsed_query, GL_QUERY_RESULT, &time_elapsed);
					GLdouble time_elapsed_double = static_cast<GLdouble>(time_elapsed);
					std::cout << "Time elapsed: " << time_elapsed << " ns = " << time_elapsed_double * 1e-6 << " ms = " << time_elapsed_double * 6e-6 << " % of draw frame = " << time_elapsed_double * 1.2e-5 << " % of physics step" << std::endl;
				}
			}
			++draw_count;

			glfwPollEvents();
		}

		game_logic::free(game_environment);
		glfwSetWindowUserPointer(window, nullptr);
	}
}
