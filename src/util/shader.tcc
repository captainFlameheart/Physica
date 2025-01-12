#pragma once
#include <glad/glad.h>
#include "util/shader.h"
#include <iostream>

// TODO: Move out non-template functions from this file to reduce #include sizes

template <typename ...File_Paths>
struct Attach
{
};

template <>
struct Attach<>
{
	static void f(GLuint const program, GLuint* const shader)
	{
	}
};

template <typename First_File_Path, typename ...Rest_File_Paths>
struct Attach<First_File_Path, Rest_File_Paths...>
{
	static void f(
		GLuint const program,
		GLuint* const shader,
		First_File_Path&& first_file_path,
		Rest_File_Paths&&... rest_file_paths
	)
	{
		*shader = util::shader::file_to_shader(first_file_path);
		glAttachShader(program, *shader);
		Attach<Rest_File_Paths...>::f(program, shader + 1, rest_file_paths...);
	}
};

template <typename ...File_Paths>
void attach(
	GLuint const program,
	GLuint *const shader,
	File_Paths&&... file_paths
)
{
	Attach<File_Paths...>::f(program, shader, file_paths...);
}

template <typename ...File_Paths>
GLuint util::shader::link_files(File_Paths&&... file_paths)
{
	GLuint const program{ glCreateProgram() };
	GLuint shaders[sizeof...(file_paths)];
	attach(program, shaders, file_paths...);
	glLinkProgram(program);
	for (GLuint const shader : shaders)
	{
		glDeleteShader(shader);
	}

	GLint link_successful;
	glGetProgramiv(program, GL_LINK_STATUS, &link_successful);
	if (link_successful == GL_FALSE)
	{
		GLint message_length = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &message_length);
		GLchar *const message{ new char[message_length] };
		glGetProgramInfoLog(program, message_length, &message_length, message);
		std::cerr << "Link error:\n" << message << "\n";
		delete[] message;
	}

	glValidateProgram(program);	// TODO: This line might not do anything useful on its own

	return program;
}
