#include "glad_glfw.h"
#include "game_logic/shader_util/query/include.h"

namespace game_logic::shader_util::query
{
	void query_shader_storage_variable_offset_top_level_array_stride_top_level_array_size
	(
		GLuint program, char const* name,
		GLint (&values)[3u]
	)
	{
		GLenum properties[]{ GL_OFFSET, GL_TOP_LEVEL_ARRAY_STRIDE, GL_TOP_LEVEL_ARRAY_SIZE };
		query
		(
			program, GL_BUFFER_VARIABLE, name,
			properties, values
		);
	}
}
