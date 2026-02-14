#include "glad_glfw.h"
#include "game_logic/shader_util/query/include.h"

namespace game_logic::shader_util::query
{
	template<GLuint property_count>
	void query
	(
		GLuint program, GLenum interface, char const* name,
		GLenum const (&properties)[property_count], GLint (&values)[property_count]
	)
	{
		query
		(
			program, interface, name,
			property_count, properties,
			property_count, nullptr, values
		);
	}
}
