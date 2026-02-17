#include "game_logic/on_framebuffer_size_changed/include.h"
#include "game_environment/environment.h"

namespace game_logic::on_framebuffer_size_changed
{
	void on_framebuffer_size_changed(game_environment::Environment& environment, int width, int height)
	{
		std::cout << "Framebuffer size changed" << std::endl;
		
		environment.state.glfw.framebuffer_width = width;
		environment.state.glfw.framebuffer_height = height;

		glViewport(0, 0, width, height);
	}
}
