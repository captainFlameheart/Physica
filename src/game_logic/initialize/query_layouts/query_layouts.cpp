#include "game_logic/initialize/query_layouts/include.h"
#include "game_environment/Environment.h"
#include "util/shader/shader.h"
#include "game_state/device_requirements/OpenGL_versions.h"
#include "game_state/entity_type_indices/entity_type_indices.h"
#include "game_state/local_sizes/include.h"
#include <string>

namespace game_logic::initialize::query_layouts
{
	void query_layouts(game_environment::Environment& environment)
	{
		// TODO: Do avoid repeates of constants and file loads

		constexpr GLuint draw_arrays_program_count = 3u;

		GLuint compute_shader{ ::util::shader::create_shader(GL_COMPUTE_SHADER) };
		::util::shader::set_shader_statically
		(
			compute_shader,
			"#version " + std::string{ ::game_state::device_requirements::OpenGL_versions::version_string } + "\n"
			"#define FIXED_DATA_RESTRICT_PLACEHOLDER restrict\n"
			"#define FIXED_DATA_BUFFER_PLACEHOLDER buffer\n"
			"#define READ_ONLY_PLACEHOLDER \n"
			"const uint fixed_data_binding = 0;\n"
			"const uint uvec4_data_binding = 1;\n"
			"const uint uint_data_binding = 2;\n"
			"const uint float_data_binding = 3;\n"
			"const uint entity_type_count = " + std::to_string(::game_state::entity_type_indices::count) + ";\n"
			"const uint dispatch_program_count = " + std::to_string(::game_state::shader_indices::tick::process_entities::count) + ";\n"
			"const uint draw_arrays_program_count = " + std::to_string(draw_arrays_program_count) + ";\n"
			,
			::util::shader::file_to_string("blocks/Fixed_Data"),
			::util::shader::file_to_string("blocks/shader_storage/uvec4_Data"),
			::util::shader::file_to_string("blocks/shader_storage/uint_Data"),
			::util::shader::file_to_string("blocks/shader_storage/float_Data"),
			::util::shader::file_to_string("dummies/compute.comp")
		);

		GLuint program{ ::util::shader::create_program(compute_shader) };
		
		::util::shader::delete_shader(compute_shader);

		query_fixed_data_layout(environment, program);
		query_uvec4_data_layout(environment, program);
		query_uint_data_layout(environment, program);
		query_float_data_layout(environment, program);

		::util::shader::delete_program(program);
	}
}
