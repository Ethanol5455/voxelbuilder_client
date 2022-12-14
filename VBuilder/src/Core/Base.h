#pragma once

#include <memory>

//#include <glad/glad.h>
//#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

#ifdef VB_DEBUG
	#if defined(VB_PLATFORM_WINDOWS)
		#define VB_DEBUGBREAK() __debugbreak()
	#elif defined(VB_PLATFORM_LINUX)
		#include <signal.h>
		#define VB_DEBUGBREAK() raise(SIGTRAP)
	#else
		#error "Platform doesn't support debugbreak yet!"
	#endif
	#define VB_ENABLE_ASSERTS
#else
	#define VB_DEBUGBREAK()
#endif

// TODO: Make this macro able to take in no arguments except condition
#ifdef VB_ENABLE_ASSERTS
	#define VB_ASSERT(x, ...) { if(!(x)) { VB_ERROR("Assertion Failed: {0}", __VA_ARGS__); VB_DEBUGBREAK(); } }
#else
	#define VB_ASSERT(x, ...) { if(!(x)) { VB_ERROR("Assertion Failed: {0}", __VA_ARGS__); } }
#endif

#define BIT(x) (1 << x)

#define VB_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)
