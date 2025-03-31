#pragma once
#include <string>
#include <type_traits>

#define util_shader_PREPROCESSOR_DIRECTIVE(identifier, arguments) "#" identifier " " arguments "\n"
#define util_shader_VERSION util_shader_PREPROCESSOR_DIRECTIVE("version", "460 core")//"#version 460 core\n"
#define util_shader_DEFINE(identifier, replacement) \
	util_shader_PREPROCESSOR_DIRECTIVE("define", identifier " " replacement) //"#define " identifier " " replacement "\n"
//#define util_shader_DEFINE(...) "#define "#__VA_ARGS__"\n"

namespace util::shader
{
	std::string file_to_string(std::string const& file_path);

	GLuint create_shader(GLenum type);

	void set_shader(
		GLuint const shader,
		GLsizei const count,
		GLchar const* const* const source,
		GLint const* const length
	);

	template<typename... Sources>
	void set_shader_statically(GLuint const shader, Sources&& ...sources);

	GLuint create_program();

	template <typename ...Shaders>
	void set_program(GLuint const program, Shaders&& ...shaders);

	template <typename ...Shaders>
	GLuint create_program(Shaders&& ...shaders);

	void delete_shader(GLuint const shader);

	void delete_program(GLuint const program);
}

#include "util/shader/shader.tcc"
