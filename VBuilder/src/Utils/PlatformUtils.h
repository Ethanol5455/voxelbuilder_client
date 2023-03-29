#pragma once

#include <string>
#include <filesystem>

#include "Core/Window.h"

class FileDialogs {
    public:
	// These return empty strings if canceled
	static std::string OpenFile(Window &window, const char *filter);
	static std::string SaveFile(Window &window, const char *filter);
	static std::string OpenDirectory(Window &window);
};