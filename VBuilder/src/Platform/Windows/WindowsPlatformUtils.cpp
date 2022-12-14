#include "vbpch.h"

#ifdef VB_PLATFORM_WINDOWS

#include "Utils/PlatformUtils.h"

#include <atlstr.h>
#include <commdlg.h>
#include <shlobj.h>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <ShellScalingApi.h>

std::string FileDialogs::OpenFile(Window& window, const char* filter)
{
	OPENFILENAMEA ofn;
	CHAR szFile[260] = { 0 };
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)window.GetNativeWindow());
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = filter;
	ofn.nFilterIndex = 1;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
	if (GetOpenFileNameA(&ofn) == TRUE)
	{
		return ofn.lpstrFile;
	}
	return std::string();
}

std::string FileDialogs::SaveFile(Window& window, const char* filter)
{
	OPENFILENAMEA ofn;
	CHAR szFile[260] = { 0 };
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)window.GetNativeWindow());
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = filter;
	ofn.nFilterIndex = 1;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
	if (GetSaveFileNameA(&ofn) == TRUE)
	{
		return ofn.lpstrFile;
	}
	return std::string();
}

std::string FileDialogs::OpenDirectory(Window& window)
{
	BROWSEINFO bi;
	wchar_t buffer[260] = { 0 };
	bi.hwndOwner = glfwGetWin32Window((GLFWwindow*)window.GetNativeWindow());
	bi.pidlRoot = NULL;
	bi.pszDisplayName = NULL;
	bi.ulFlags = BIF_STATUSTEXT; //BIF_EDITBOX
	bi.lpszTitle = (LPCWSTR)"TestTitle";
	bi.lParam = (LPARAM)"C:\\";
	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
	if (pidl)
	{
		if (SHGetPathFromIDList(pidl, buffer))
		{
			//std::string f = s((LPCTSTR)buffer);
			//return std(buffer, buffer.GetLength());
			//buffer.
		}
	}
	return std::string();
}

#endif