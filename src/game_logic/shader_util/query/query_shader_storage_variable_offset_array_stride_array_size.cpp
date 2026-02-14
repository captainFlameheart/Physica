#include "glad_glfw.h"
#include "game_logic/shader_util/query/include.h"

namespace game_logic::shader_util::query
{
	void query_shader_storage_variable_offset_array_stride_array_size
	(
		GLuint program, char const* name,
		GLint (&values)[3u]
	)
	{
		GLenum properties[]{ GL_OFFSET, GL_ARRAY_STRIDE, GL_ARRAY_SIZE };
		query
		(
			program, GL_BUFFER_VARIABLE, name,
			properties, values
		);
	}
}
