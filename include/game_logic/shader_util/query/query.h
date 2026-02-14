#pragma once
#include "glad_glfw.h"

namespace game_logic::shader_util::query
{
	void query
	(
		GLuint program, GLenum interface, char const* name,
		GLsizei property_count, GLenum const* properties,
		GLsizei buffer_size, GLsizei* length, GLint* values
	);

	template<GLuint property_count>
	void query
	(
		GLuint program, GLenum interface, char const* name,
		GLenum const (&properties)[property_count], GLint (&values)[property_count]
	);
}

#include "game_logic/shader_util/query/query.tcc"
