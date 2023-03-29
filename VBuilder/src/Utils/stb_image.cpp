#include "vbpch.h"

#ifdef VB_DEBUG
#define STBI_FAILURE_USERMSG
#else
#define STBI_NO_FAILURE_STRINGS
#endif

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"