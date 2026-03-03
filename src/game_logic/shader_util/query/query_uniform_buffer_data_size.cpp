#include "glad_glfw.h"
#include "game_logic/shader_util/query/include.h"

namespace game_logic::shader_util::query
{
	void query_uniform_buffer_data_size
	(
		GLuint program, char const* name,
		GLint (&values)[1u]
	)
	{
		GLenum properties[]{ GL_BUFFER_DATA_SIZE };
		query
		(
			program, GL_UNIFORM_BLOCK, name,
			properties, values
		);
	}
}
