#include "glad_glfw.h"

namespace game_state::bindings::shader_storage
{
	enum bindings : GLuint
	{
		fixed_data,
		uvec4_data,
		vec4_data,
		uvec2_data,
		vec2_data,
		uint_data,
		float_data,

		count
	};
	/*constexpr GLuint fixed_data{0u};
	constexpr GLuint uvec4_data{ 1u };
	constexpr GLuint vec4_data{ 2u };
	constexpr GLuint uvec2_data{ 3u };
	constexpr GLuint vec2_data{ 4u };
	constexpr GLuint uint_data{ 5u };
	constexpr GLuint float_data{ 6u };*/
}
