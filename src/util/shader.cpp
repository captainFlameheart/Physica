#include <glad/glad.h>
#include "util/shader.h"
#include <fstream>
#include <iterator>
#include <iostream>
#include <string>

std::string util::shader::file_to_string(std::string const &file_path)
{
	std::ifstream file_stream{ RESOURCES_PATH + file_path };
	if (!file_stream.is_open())
	{
		std::cerr << "Failed to open \"" << file_path << '"' << std::endl;
	}
	return std::string{ std::istreambuf_iterator<char>{file_stream}, std::istreambuf_iterator<char>{} };
}

GLuint util::shader::string_to_shader(std::string const &string, GLenum const shader_type)
{
	GLuint shaderId{ glCreateShader(shader_type) };
	GLchar const *const c_string{ string.c_str() };
	glShaderSource(shaderId, 1, &c_string, nullptr);
	glCompileShader(shaderId);

	GLint compilation_successful;
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compilation_successful);
	if (compilation_successful == GL_FALSE)
	{
		GLint message_length;
		glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &message_length);
		GLchar *const message{ new char[message_length] };
		glGetShaderInfoLog(shaderId, message_length, &message_length, message);
		std::cerr << "Error compiling shader:\n" << message << std::endl;
		delete[] message;
	}

	return shaderId;
}

GLenum util::shader::file_to_shader_type(std::string const &file_path)
{
	if (file_path.ends_with(".vert"))
	{
		return GL_VERTEX_SHADER;
	}
	if (file_path.ends_with(".frag"))
	{
		return GL_FRAGMENT_SHADER;
	}
	return GL_DONT_CARE;
}

GLuint util::shader::file_to_shader(std::string const &file_path)
{
	std::string const string{ file_to_string(file_path) };
	GLenum const shader_type{ file_to_shader_type(file_path) };
	GLuint const shader{ string_to_shader(string, shader_type) };
	return shader;
}
