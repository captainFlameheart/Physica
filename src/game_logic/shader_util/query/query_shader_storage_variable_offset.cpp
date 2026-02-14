#include "glad_glfw.h"
#include "game_logic/shader_util/query/include.h"

namespace game_logic::shader_util::query
{
	void query_shader_storage_variable_offset
	(
		GLuint program, char const* name,
		GLint (&values)[1u]
	)
	{
		GLenum properties[]{ GL_OFFSET };
		query
		(
			program, GL_BUFFER_VARIABLE, name,
			properties, values
		);
	}
}
