#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <openglDebug.h>
#include <demoShaderLoader.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include "util/string.h"
#include "game_loop.h"
#include "test.cpp"

#define RELEASE 0
#define DEBUG 1

#define MODE DEBUG

#define USE_GPU_ENGINE 1
extern "C"
{
	__declspec(dllexport) unsigned long NvOptimusEnablement = USE_GPU_ENGINE;
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = USE_GPU_ENGINE;
}

#define OPENGL_NO_DEBUG 0
#define OPENGL_ASYNCH_DEBUG 1
#define OPENGL_SYNCH_DEBUG 2

#define OPENGL_DEBUG_MODE MODE == DEBUG ? OPENGL_SYNCH_DEBUG : OPENGL_NO_DEBUG

static void on_key_event(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, GLFW_TRUE);
			break;
		case GLFW_KEY_F11:
			GLFWmonitor *monitor{ glfwGetPrimaryMonitor() };
			GLFWvidmode const *video_mode{ glfwGetVideoMode(monitor) };
			if (glfwGetWindowMonitor(window) == nullptr)
			{
				glfwSetWindowMonitor(window, monitor, 0, 0, video_mode->width, video_mode->height, video_mode->refreshRate);
			}
			else
			{
				int const width{ 640 }, height{ 320 };
				int const x{ (video_mode->width - width) / 2 }, y{ (video_mode->height - height) / 2 };
				glfwSetWindowMonitor(window, nullptr, x, y, width, height, GLFW_DONT_CARE);
			}
			break;
		}
	}
}

void on_glfw_error(int error_code, const char* description)
{
	std::cerr << "GLFW error " << util::string::to_hex(error_code) << ": " << description << std::endl;
}

int main(void)
{
	glfwSetErrorCallback(on_glfw_error);

	if (glfwInit() == GLFW_FALSE)
	{
		return EXIT_FAILURE;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

#if OPENGL_DEBUG_MODE != OPENGL_NO_DEBUG
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif
	GLFWmonitor* monitor{ glfwGetPrimaryMonitor() };
	GLFWvidmode const* video_mode{ glfwGetVideoMode(monitor) };
	glfwWindowHint(GLFW_RED_BITS, video_mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, video_mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, video_mode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, video_mode->refreshRate);

	GLFWwindow* window = glfwCreateWindow(video_mode->width, video_mode->height, "Physica", monitor, NULL);
	if (window == nullptr)
	{
		glfwTerminate();
		return EXIT_FAILURE;
	}

	glfwSetKeyCallback(window, on_key_event);

	glfwMakeContextCurrent(window);

	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	
	glfwSwapInterval(1);

	#if OPENGL_DEBUG_MODE != OPENGL_NO_DEBUG
		glEnable(GL_DEBUG_OUTPUT);
		#if OPENGL_DEBUG_MODE == OPENGL_SYNCH_DEBUG
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		#endif
		glDebugMessageCallback(glDebugOutput, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	#endif

	run_game_loop(window);

	glfwDestroyWindow(window);
	glfwTerminate();
	return EXIT_SUCCESS;
}
