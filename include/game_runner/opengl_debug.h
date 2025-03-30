#pragma once
#include <glad/glad.h>

namespace game_runner
{
	void GLAPIENTRY gl_debug_output
	(
		GLenum source,
		GLenum type,
		unsigned int id,
		GLenum severity,
		GLsizei length,
		const char* message,
		const void* userParam
	);
}