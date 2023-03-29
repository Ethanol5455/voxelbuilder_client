#include "vbpch.h"

#ifdef VB_PLATFORM_LINUX

#include "Utils/PlatformUtils.h"

//#include <GLFW/glfw3.h>
//#define GLFW_EXPOSE_NATIVE_X11
//#include <GLFW/glfw3native.h>

std::string FileDialogs::OpenFile(Window &window, const char *filter)
{
	char filename[1024];
	FILE *f = popen("zenity --file-selection", "r");
	fgets(filename, 1024, f);
	return filename;
}

std::string FileDialogs::SaveFile(Window &window, const char *filter)
{
	char filename[1024];
	FILE *f = popen("zenity --save", "r");
	fgets(filename, 1024, f);
	return filename;
}

std::string FileDialogs::OpenDirectory(Window &window)
{
	char filename[1024];
	FILE *f = popen("zenity --file-selection --directory", "r");
	fgets(filename, 1024, f);
	return filename;
	return std::string();
}

#endif