#pragma once
#include "game_logic/GLFW/include.h"

namespace game_logic::GLFW
{
	void toggle_fullscreen(game_environment::Environment& environment)
	{
		GLFWmonitor* window_monitor{ glfwGetWindowMonitor(environment.window) };
		if (window_monitor == nullptr)
		{
			GLFWmonitor* primary_monitor{ glfwGetPrimaryMonitor() };
			GLFWvidmode const* video_mode{ glfwGetVideoMode(primary_monitor) };
			glfwSetWindowMonitor(environment.window, primary_monitor, 0, 0, video_mode->width, video_mode->height, video_mode->refreshRate);
		}
		else
		{
			glfwSetWindowMonitor(environment.window, nullptr, 100, 100, 600, 300, GLFW_DONT_CARE);
		}
	}
}
