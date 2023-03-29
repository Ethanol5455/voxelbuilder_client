#include "vbpch.h"

#include "Camera3D.h"

#include "glm/gtc/matrix_transform.hpp"

Camera3D::Camera3D(const glm::vec3 &position)
	: m_Position(position)
	, m_Front({ 0.0f, 0.0f, -1.0f })
	, m_Pitch(0.0f)
	, m_Yaw(0.0f)
{
	m_PreviousMousePosition = Input::GetMousePosition();
	m_Speed = 10.0f;
	m_Sensitivity = 0.25f;
	m_View = glm::mat4(1.0f);
	CalculateView();
}

void Camera3D::Update(float deltaTime)
{
	float speed = m_Speed * deltaTime;

	if (Input::IsKeyPressed(GLFW_KEY_LEFT_SHIFT))
		speed *= 5;
	if (Input::IsKeyPressed(GLFW_KEY_LEFT_CONTROL))
		speed *= 5;
	if (Input::IsKeyPressed(GLFW_KEY_LEFT_ALT))
		speed /= 5;

	glm::vec3 prevPosition = m_Position;

	if (Input::IsKeyPressed(GLFW_KEY_W))
		m_Position += m_Front * speed;
	if (Input::IsKeyPressed(GLFW_KEY_S))
		m_Position -= m_Front * speed;
	if (Input::IsKeyPressed(GLFW_KEY_A))
		m_Position -=
			glm::normalize(glm::cross(m_Front, m_UpDir)) * speed;
	if (Input::IsKeyPressed(GLFW_KEY_D))
		m_Position +=
			glm::normalize(glm::cross(m_Front, m_UpDir)) * speed;

	if (Input::IsKeyPressed(GLFW_KEY_SPACE))
		m_Position.y += speed;
	if (Input::IsKeyPressed(GLFW_KEY_C))
		m_Position.y -= speed;

	glm::vec2 currentPosition = Input::GetMousePosition();
	glm::vec2 mouseDifference = currentPosition - m_PreviousMousePosition;
	m_PreviousMousePosition = currentPosition;

	if (mouseDifference != glm::vec2(0.0f) || m_Position != prevPosition) {
		//VB_INFO("({0}, {1}, {2})", m_Position.x, m_Position.y, m_Position.z);

		m_Yaw += mouseDifference.x * m_Sensitivity;
		m_Pitch -= mouseDifference.y * m_Sensitivity;

		if (m_Pitch > 89.9f)
			m_Pitch = 89.9f;
		else if (m_Pitch < -89.9f)
			m_Pitch = -89.9f;

		CalculateView();
	}
}

void Camera3D::CalculateView()
{
	glm::vec3 direction;
	direction.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
	direction.y = sin(glm::radians(m_Pitch));
	direction.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
	m_Front = glm::normalize(direction);

	glm::vec3 right = glm::normalize(
		glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), direction));

	glm::vec3 up = glm::cross(direction, right);

	m_View = glm::lookAt(m_Position, m_Position + m_Front, m_UpDir);
}
