#include "vbpch.h"
#include "Window.h"




static void GLFWErrorCallback(int error, const char* description)
{
	VB_ERROR("GLFW Error[{0}]: {1}", error, description);
}

Window::Window()
{
	Create("Window", 500, 500);
}

Window::Window(const std::string& title, uint32_t width, uint32_t height)
{
	Create(title, width, height);
}

Window::~Window()
{
	glfwDestroyWindow(m_Window);
}

void Window::Create(const std::string& title, uint32_t width, uint32_t height)
{
	m_Data.width = width;
	m_Data.height = height;
	m_Data.xScale = 1.0f;
	m_Data.yScale = 1.0f;
	m_Data.title = title;

	VB_ASSERT(glfwInit(), "Unable to initialize GLFW");
	glfwSetErrorCallback(GLFWErrorCallback);

	//glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
	glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);
#ifdef VB_DEBUG
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#else
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif

	m_Window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

	VB_ASSERT(m_Window, "Unable to create window!");

	glfwGetMonitorContentScale(glfwGetPrimaryMonitor(), &m_Data.xScale, &m_Data.yScale);

	m_Context = std::make_shared<GraphicsContext>(m_Window);
	m_Context->Init();
#ifdef VB_DEBUG
	VB_INFO("Debug Mode");
#else
	VB_INFO("Release Mode");
#endif

	glfwSetWindowUserPointer(m_Window, &m_Data);
	SetVSync(true);
	SetFullscreen(false);



	// Set GLFW Callbacks

	glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.width = width;
			data.height = height;

			WindowResizeEvent event(width, height);
			data.EventCallback(event);
		});

	glfwSetWindowContentScaleCallback(m_Window, [](GLFWwindow* window, float xscale, float yscale)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.xScale = xscale;
			data.yScale = yscale;

			WindowContentScaleChangedEvent event(xscale, yscale);
			data.EventCallback(event);
		});

	glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			WindowCloseEvent event;
			data.EventCallback(event);
		});

	glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
			case GLFW_PRESS:
			{
				KeyPressedEvent event(key, 0);
				data.EventCallback(event);
				break;
			}
			case GLFW_RELEASE:
			{
				KeyReleasedEvent event(key);
				data.EventCallback(event);
				break;
			}
			case GLFW_REPEAT:
			{
				KeyPressedEvent event(key, 1);
				data.EventCallback(event);
				break;
			}
			}
		});

	glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
			case GLFW_PRESS:
			{
				MouseButtonPressedEvent event(button);
				data.EventCallback(event);
				break;
			}
			case GLFW_RELEASE:
			{
				MouseButtonReleasedEvent event(button);
				data.EventCallback(event);
				break;
			}
			}
		});

	glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			MouseScrolledEvent event((float)xOffset, (float)yOffset);
			data.EventCallback(event);
		});

	glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			MouseMovedEvent event((float)xPos, (float)yPos);
			data.EventCallback(event);
		});
}

GLFWwindow* Window::GetNativeWindow()
{
	return m_Window;
}

void Window::Clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::Clear(glm::vec4 color)
{
	glClearColor(color.r, color.g, color.b, color.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::PollEvents()
{
	glfwPollEvents();
}

void Window::SwapBuffers()
{
	m_Context->SwapBuffers();
}

void Window::Close()
{
	glfwSetWindowShouldClose(m_Window, GLFW_TRUE);
}

bool Window::ShouldClose()
{
	return glfwWindowShouldClose(m_Window);
}

void Window::SetTitle(const std::string& title)
{
	m_Data.title = title;
	glfwSetWindowTitle(m_Window, title.c_str());
}

float Window::GetWidth() const
{
	if (m_Data.width == 0)
		return 1.0f;
	return (float)m_Data.width;
}

float Window::GetHeight() const
{
	if (m_Data.height == 0)
		return 1.0f;
	return (float)m_Data.height;
}

float Window::GetAspectRatio() const
{
	if (m_Data.width == 0 || m_Data.height == 0)
		return 1.0f;
	return (float)m_Data.width / (float)m_Data.height;
}

void Window::SetVSync(bool enabled)
{
	if (enabled)
		glfwSwapInterval(1);
	else
		glfwSwapInterval(0);

	m_Data.vSync = enabled;
}

bool Window::IsVSync() const
{
	return m_Data.vSync;
}

void Window::Maximize()
{
	glfwMaximizeWindow(m_Window);
}

void Window::SetFullscreen(bool enabled)
{
	if (enabled)
	{
		glfwSetWindowMonitor(m_Window, glfwGetPrimaryMonitor(), 0, 0, m_Data.width, m_Data.height, 60);
	}
	else
	{
		int x, y;
		glfwGetWindowPos(m_Window, &x, &y);
		glfwSetWindowMonitor(m_Window, nullptr, x, y, m_Data.width, m_Data.height, 60);
	}

	SetVSync(IsVSync());
	m_Data.fullscreen = enabled;
}

bool Window::IsFullscreen() const
{
	return m_Data.fullscreen;
}

void Window::SetResolution(const glm::vec2& resolution)
{
	m_Data.width = (uint32_t)resolution.x;
	m_Data.height = (uint32_t)resolution.y;
	glfwSetWindowSize(m_Window, m_Data.width, m_Data.height);
}

glm::vec2 Window::GetResolution() const
{
	return glm::vec2(m_Data.width, m_Data.height);
}

void Window::SetPosition(const glm::vec2& position)
{
	glfwSetWindowPos(m_Window, (int)position.x, (int)position.y);
}

glm::vec2 Window::GetPosition()
{
	int x;
	int y;
	glfwGetWindowPos(m_Window, &x, &y);
	return glm::vec2(x, y);
}