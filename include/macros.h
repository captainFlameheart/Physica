#pragma once
#include <iostream>

#define SHADER_PATH RESOURCES_PATH "/shaders/"

#define RELEASE 0
#define DEBUG 1

#define MODE DEBUG

#define USE_GPU_ENGINE 1

#define OPENGL_NO_DEBUG 0
#define OPENGL_ASYNCH_DEBUG 1
#define OPENGL_SYNCH_DEBUG 2

#define OPENGL_DEBUG_MODE MODE == DEBUG ? OPENGL_SYNCH_DEBUG : OPENGL_NO_DEBUG

#if MODE == RELEASE
#define DEBUG_LOG(message)
#elif MODE == DEBUG
#define DEBUG_LOG(message) std::cout << message << std::endl
#endif
