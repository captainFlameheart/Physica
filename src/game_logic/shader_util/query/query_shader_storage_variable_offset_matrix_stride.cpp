#include "glad_glfw.h"
#include "game_logic/shader_util/query/include.h"

namespace game_logic::shader_util::query
{
	void query_shader_storage_variable_offset_matrix_stride
	(
		GLuint program, char const* name,
		GLint (&values)[2u]
	)
	{
		GLenum properties[]{ GL_OFFSET, GL_MATRIX_STRIDE };
		query
		(
			program, GL_BUFFER_VARIABLE, name,
			properties, values
		);
	}
}
