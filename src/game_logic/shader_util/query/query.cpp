#include "glad_glfw.h"
#include "game_logic/shader_util/query/include.h"

namespace game_logic::shader_util::query
{
	void query
	(
		GLuint program, GLenum interface, char const* name,
		GLsizei property_count, GLenum const* properties,
		GLsizei buffer_size, GLsizei* length, GLint* values
	)
	{
		GLuint index{ glGetProgramResourceIndex(program, interface, name) };
		glGetProgramResourceiv
		(
			program, interface, index,
			property_count, properties,
			buffer_size, length, values
		);
	}
}
