#pragma once
#include <glad/glad.h>
#include "util/shader/shader.h"
#include <iostream>
#include <type_traits>

// TODO: Move out non-template functions from this file to reduce #include sizes

template<typename T>
concept Has_C_Str_Method = requires(T a)
{
	a.c_str();
};


template<typename Source>
struct To_C_String
{
	static GLchar const* f(Source&& source)
	{
		return source;
	}
};


template <Has_C_Str_Method Source>
struct To_C_String<Source>
{
	static GLchar const* f(Source&& source)
	{
		return source.c_str();
	}
};

template <typename ...Sources>
struct SetSourcePointers
{
	static void f(GLchar const** pointer)
	{
	}
};

template <typename First_Source, typename ...Rest_Sources>
struct SetSourcePointers<First_Source, Rest_Sources...>
{
	static void f(GLchar const** pointer, First_Source&& first_source, Rest_Sources&&... rest_sources)
	{
		*pointer = To_C_String<First_Source>::f(std::forward<First_Source>(first_source));
		SetSourcePointers<Rest_Sources...>::f(pointer + 1, std::forward<Rest_Sources>(rest_sources)...);
	}
};

template<typename... Sources>
void util::shader::set_shader_statically(GLuint const shader, Sources&& ...sources)
{
	// TODO: Allow optional length per source string instead of null-character termination
	GLchar const* pointers[sizeof...(sources)];
	SetSourcePointers<Sources...>::f(pointers, std::forward<Sources>(sources)...);
	util::shader::set_shader(shader, sizeof...(sources), pointers, nullptr);
}

template <typename ...Shaders>
struct Attach
{
};

template <>
struct Attach<>
{
	static void f(GLuint const program)
	{
	}
};

template <typename First_Shader, typename ...Rest_Shaders>
struct Attach<First_Shader, Rest_Shaders...>
{
	static void f(GLuint const program, First_Shader&& first_shader, Rest_Shaders&&... rest_shaders)
	{
		glAttachShader(program, first_shader);
		Attach<Rest_Shaders...>::f(program, rest_shaders...);
	}
};

template <typename ...Shaders>
void attach(GLuint const program, Shaders&&... shaders)
{
	Attach<Shaders...>::f(program, shaders...);
}

template <typename ...Shaders>
struct Detach
{
};

template <>
struct Detach<>
{
	static void f(GLuint const program)
	{
	}
};

template <typename First_Shader, typename ...Rest_Shaders>
struct Detach<First_Shader, Rest_Shaders...>
{
	static void f(GLuint const program, First_Shader&& first_shader, Rest_Shaders&&... rest_shaders)
	{
		glDetachShader(program, first_shader);
		Attach<Rest_Shaders...>::f(program, rest_shaders...);
	}
};

template <typename ...Shaders>
void detach(GLuint const program, Shaders&&... shaders)
{
	Detach<Shaders...>::f(program, shaders...);
}

template <typename ...Shaders>
void util::shader::set_program(GLuint const program, Shaders&&... shaders)
{
	attach(program, shaders...);
	glLinkProgram(program);
	detach(program, shaders...);

	GLint link_successful;
	glGetProgramiv(program, GL_LINK_STATUS, &link_successful);
	if (link_successful == GL_FALSE)
	{
		GLint message_length = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &message_length);
		GLchar* const message{ new char[message_length] };
		glGetProgramInfoLog(program, message_length, &message_length, message);
		std::cerr << "Link error:\n" << message << "\n";
		delete[] message;
	}

	glValidateProgram(program);	// TODO: This line might not do anything useful on its own
}

template <typename ...Shaders>
GLuint util::shader::create_program(Shaders&&... shaders)
{
	GLuint const program{ util::shader::create_program() };
	util::shader::set_program(program, std::forward<Shaders>(shaders)...);
	return program;
}
