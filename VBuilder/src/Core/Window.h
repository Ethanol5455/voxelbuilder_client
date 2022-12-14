#pragma once

#include <functional>

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "Events/ApplicationEvent.h"
#include "Events/MouseEvent.h"
#include "Events/KeyEvent.h"
#include "Renderer/GraphicsContext.h"


class Window
{
public:
	using EventCallbackFn = std::function<void(Event&)>;

	Window();
	Window(const std::string& title, uint32_t width, uint32_t height);
	~Window();


	GLFWwindow* GetNativeWindow();

	void Clear();
	void Clear(glm::vec4 color);

	void PollEvents();
	void SwapBuffers();

	void Close();
	bool ShouldClose();

	const std::string& GetTitle() { return m_Data.title; }
	void SetTitle(const std::string& title);

	float GetWidth() const;
	float GetHeight() const;
	float GetAspectRatio() const;

	float GetXContentScale() { return m_Data.xScale; }
	float GetYContentScale() { return m_Data.yScale; }

	void SetVSync(bool enabled);
	bool IsVSync() const;

	void Maximize();

	void SetFullscreen(bool enabled);
	bool IsFullscreen() const;

	void SetResolution(const glm::vec2& resolution);
	glm::vec2 GetResolution() const;

	void SetPosition(const glm::vec2& position);
	glm::vec2 GetPosition();

	inline void SetEventCallback(const EventCallbackFn& callback) { m_Data.EventCallback = callback; }

	operator GLFWwindow* () { return m_Window; }

private:
	void Create(const std::string& title, uint32_t width, uint32_t height);

	GLFWwindow* m_Window;
	std::shared_ptr<GraphicsContext> m_Context;

	struct WindowData
	{
		std::string title;
		uint32_t width, height;
		float xScale, yScale;
		bool vSync;
		bool fullscreen;

		EventCallbackFn EventCallback;
	};

	WindowData m_Data;

};
