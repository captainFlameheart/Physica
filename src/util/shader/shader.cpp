#include <glad/glad.h>
#include "util/shader/shader.h"
#include <fstream>
#include <iterator>
#include <iostream>
#include <string>
#include "macros/macros.h"

namespace util::shader
{
	std::string file_to_string(std::string const &file_path)
	{
		std::ifstream file_stream{ SHADER_PATH + file_path};
		if (!file_stream.is_open())
		{
			std::cerr << "Failed to open \"" << file_path << '"' << std::endl;
		}
		return std::string{ std::istreambuf_iterator<char>{file_stream}, std::istreambuf_iterator<char>{} };
	}

	GLuint create_shader(GLenum type)
	{
		return glCreateShader(type);
	}

	void set_shader(
		GLuint const shader,
		GLsizei const count,
		GLchar const *const *const source,
		GLint const* const length
	)
	{
		glShaderSource(shader, count, source, length);
		glCompileShader(shader);

		GLint compilation_successful;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compilation_successful);
		if (compilation_successful == GL_FALSE)
		{
			GLint message_length;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &message_length);
			GLchar* const message{ new char[message_length] };
			glGetShaderInfoLog(shader, message_length, &message_length, message);
			std::cerr << "Error compiling shader:\n" << message << std::endl;
			delete[] message;
		}
	}

	GLuint create_program()
	{
		return glCreateProgram();
	}

	void delete_shader(GLuint const shader)
	{
		glDeleteShader(shader);
	}

	void delete_program(GLuint const program)
	{
		glDeleteProgram(program);
	}
}
