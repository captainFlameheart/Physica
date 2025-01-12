#pragma once
#include <string>

namespace util
{
	namespace shader
	{
		std::string file_to_string(std::string const &file_path);

		GLuint string_to_shader(std::string const &string, GLenum const shader_type);

		GLenum file_to_shader_type(std::string const &file_path);

		GLuint file_to_shader(std::string const &file_path);

		template <typename ...File_Paths>
		GLuint link_files(File_Paths&&... file_paths);
	}
}

#include "util/shader.tcc"
