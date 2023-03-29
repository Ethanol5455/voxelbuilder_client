#pragma once

#include <GLFW/glfw3.h>

#include "Core/Window.h"

class Input {
    public:
	static bool IsMouseButtonPressed(int button);
	static glm::vec2 GetMousePosition();

	static bool IsKeyPressed(int key);

	static void SetWindow(Window *window)
	{
		m_Window = window;
	}

    private:
	static Window *m_Window;

	static double MouseXPos;
	static double MouseYPos;
};