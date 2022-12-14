#include "vbpch.h"
#include "GraphicsContext.h"

#include <glad/glad.h>


GraphicsContext::GraphicsContext(GLFWwindow* windowHandle)
	: m_WindowHandle(windowHandle)
{
	VB_ASSERT(windowHandle, "Window handle is null!")
}

static void OpenGLLogMessage(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:
		VB_ERROR("[OpenGL Debug HIGH] {0}", message);
		VB_ASSERT(false, "GL_DEBUG_SEVERITY_HIGH");
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		VB_WARN("[OpenGL Debug MEDIUM] {0}", message);
		break;
	case GL_DEBUG_SEVERITY_LOW:
		VB_WARN("[OpenGL Debug LOW] {0}", message);
		break;
	case GL_DEBUG_SEVERITY_NOTIFICATION:
		//VB_TRACE("[OpenGL Debug NOTIFICATION] {0}", message);
		break;
	}
}

void GraphicsContext::Init()
{
	glfwMakeContextCurrent(m_WindowHandle);
	VB_ASSERT(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress), "Could not initialize GLAD");

	glDebugMessageCallback(OpenGLLogMessage, nullptr);
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

	VB_INFO("OpenGL Info:");
	VB_INFO("  Vendor: {0}", glGetString(GL_VENDOR));
	VB_INFO("  Renderer: {0}", glGetString(GL_RENDERER));
	VB_INFO("  Version: {0}", glGetString(GL_VERSION));
	VB_INFO("GLFW Version: {0}", glfwGetVersionString());

	VB_ASSERT(GLVersion.major > 4 || (GLVersion.major == 4 && GLVersion.minor >= 3), "VBuilder requires at least OpenGL version 4.3!");
}

void GraphicsContext::SwapBuffers()
{
	glfwSwapBuffers(m_WindowHandle);
}