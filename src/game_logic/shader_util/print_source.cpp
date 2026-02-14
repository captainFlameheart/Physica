#include "game_logic/shader_util/include.h"
#include <iostream>

namespace game_logic::shader_util
{
	void print_source(GLuint shader)
	{
		GLint source_length;
		glGetShaderiv(shader, GL_SHADER_SOURCE_LENGTH, &source_length);
		GLchar* source = new GLchar[source_length];
		glGetShaderSource(shader, source_length, nullptr, source);
		std::cout << source << std::endl;
		delete[] source;
	}
}
