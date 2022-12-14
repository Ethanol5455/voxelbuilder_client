#pragma once

#include <GLFW/glfw3.h>


class GraphicsContext
{
public:
	GraphicsContext(GLFWwindow* windowHandle);

	void Init();
	void SwapBuffers();
private:
	GLFWwindow* m_WindowHandle;
};