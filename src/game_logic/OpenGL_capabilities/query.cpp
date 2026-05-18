#include "glad_glfw.h"
#include "game_logic/OpenGL_capabilities/include.h"
#include <iostream>

namespace game_logic::OpenGL_capabilities
{
	void query(game_state::OpenGL_capabilities::State& state)
	{
		glGetIntegerv(GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS, &state.max_compute_shader_storage_blocks);
		glGetIntegerv(GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS, &state.max_combined_shader_storage_blocks);
		glGetIntegerv(GL_MAX_COMPUTE_UNIFORM_BLOCKS, &state.max_compute_uniform_blocks);
		glGetIntegerv(GL_MAX_COMBINED_UNIFORM_BLOCKS, &state.max_combined_uniform_blocks);
		glGetIntegerv(GL_MAX_DRAW_BUFFERS, &state.max_draw_buffers);
		glGetIntegerv(GL_MAX_DUAL_SOURCE_DRAW_BUFFERS, &state.max_dual_source_draw_buffers);
		glGetIntegerv(GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS, &state.max_fragment_shader_storage_blocks);
		glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_BLOCKS, &state.max_fragment_uniform_blocks);
		glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &state.max_shader_storage_buffer_bindings);
		glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &state.max_uniform_buffer_bindings);
		glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &state.max_uniform_block_size);
		glGetIntegerv(GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS, &state.max_vertex_shader_storage_blocks);
		glGetIntegerv(GL_MAX_VERTEX_UNIFORM_BLOCKS, &state.max_vertex_uniform_blocks);
		glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT , &state.uniform_buffer_offset_alignment);

		GLubyte const* renderer{ glGetString(GL_RENDERER) };
		GLubyte const* vendor{ glGetString(GL_VENDOR) };
		GLubyte const* version{ glGetString(GL_VERSION) };
		GLubyte const* shading_language_version{ glGetString(GL_SHADING_LANGUAGE_VERSION) };

		GLint major_version;
		glGetIntegerv(GL_MAJOR_VERSION, &major_version);
		GLint minor_version;
		glGetIntegerv(GL_MINOR_VERSION, &minor_version);

		std::cout << "renderer: " << renderer << std::endl;
		std::cout << "vendor: " << vendor << std::endl;
		std::cout << "version: " << version << std::endl;
		std::cout << "shading_language_version: " << shading_language_version << std::endl;

		std::cout << "major_version: " << major_version << std::endl;
		std::cout << "minor_version: " << minor_version << std::endl;
	}
}
