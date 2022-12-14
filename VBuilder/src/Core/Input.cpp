#include "vbpch.h"
#include "Input.h"

Window* Input::m_Window;

double Input::MouseXPos;
double Input::MouseYPos;

bool Input::IsMouseButtonPressed(int button)
{
	return glfwGetMouseButton(m_Window->GetNativeWindow(), button);
}

glm::vec2 Input::GetMousePosition()
{
	glfwGetCursorPos(m_Window->GetNativeWindow(), &MouseXPos, &MouseYPos);
	return glm::vec2(MouseXPos, MouseYPos);
}

bool Input::IsKeyPressed(int key)
{
	if (glfwGetKey(m_Window->GetNativeWindow(), key) == GLFW_PRESS)
	{
		return true;
	}
	return false;
}