#include <openglDebug.h>
#include <iostream>

//https://learnopengl.com/In-Practice/Debugging
void GLAPIENTRY glDebugOutput(GLenum source,
	GLenum type,
	unsigned int id,
	GLenum severity,
	GLsizei length,
	const char *message,
	const void *userParam)
{
	// ignore non-significant error/warning codes
	if (id == 131169 || id == 131185 || id == 131218 || id == 131204
		|| id == 131222
		|| id == 131140 //dittering error
		) return;
	if (type == GL_DEBUG_TYPE_PERFORMANCE) return;

	std::ostream& output_stream{id == GL_DEBUG_TYPE_ERROR ? std::cerr : std::cout};

	output_stream << "---------------" << std::endl;
	output_stream << "Debug message (" << id << "): " << message << std::endl;

	switch (source)
	{
	case GL_DEBUG_SOURCE_API:             output_stream << "Source: API"; break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   output_stream << "Source: Window System"; break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER: output_stream << "Source: Shader Compiler"; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:     output_stream << "Source: Third Party"; break;
	case GL_DEBUG_SOURCE_APPLICATION:     output_stream << "Source: Application"; break;
	case GL_DEBUG_SOURCE_OTHER:           output_stream << "Source: Other"; break;
	} output_stream << std::endl;

	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:               output_stream << "Type: Error"; break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: output_stream << "Type: Deprecated Behaviour"; break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  output_stream << "Type: Undefined Behaviour"; break;
	case GL_DEBUG_TYPE_PORTABILITY:         output_stream << "Type: Portability"; break;
	case GL_DEBUG_TYPE_PERFORMANCE:         output_stream << "Type: Performance"; break;
	case GL_DEBUG_TYPE_MARKER:              output_stream << "Type: Marker"; break;
	case GL_DEBUG_TYPE_PUSH_GROUP:          output_stream << "Type: Push Group"; break;
	case GL_DEBUG_TYPE_POP_GROUP:           output_stream << "Type: Pop Group"; break;
	case GL_DEBUG_TYPE_OTHER:               output_stream << "Type: Other"; break;
	} output_stream << std::endl;

	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:         output_stream << "Severity: high"; break;
	case GL_DEBUG_SEVERITY_MEDIUM:       output_stream << "Severity: medium"; break;
	case GL_DEBUG_SEVERITY_LOW:          output_stream << "Severity: low"; break;
	case GL_DEBUG_SEVERITY_NOTIFICATION: output_stream << "Severity: notification"; break;
	};

}