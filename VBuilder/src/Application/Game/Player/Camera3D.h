#pragma once

#include "Core/Input.h"

class Camera3D
{
public:
	Camera3D(const glm::vec3& position = { 0.0f, 0.0f, 0.0f });
	~Camera3D() = default;

	void SetSpeed(float speed) { m_Speed = speed; }
	void SetSensitivity(float sensitivity) { m_Sensitivity = sensitivity; }

	void Update(float deltaTime);

	const glm::vec3 GetPosition() const { return m_Position; }
	void SetPosition(const glm::vec3& position) { m_Position = position; CalculateView(); }
	void Move(const glm::vec3& distance) { m_Position += distance; CalculateView(); }

	void SetPitch(float pitch) { m_Pitch = pitch; CalculateView(); }
	float GetPitch() { return m_Pitch; }
	void SetYaw(float yaw) { m_Yaw = yaw; CalculateView(); }
	float GetYaw() { return m_Yaw; }

	const glm::vec3& GetFront() { return m_Front; }

	const glm::mat4 GetView() const { return m_View; }

private:
	void CalculateView();
private:
	float m_Speed;
	float m_Sensitivity;
	glm::vec2 m_PreviousMousePosition;

	glm::mat4 m_View;

	glm::vec3 m_Position;

	glm::vec3 m_Front;

	float m_Yaw;
	float m_Pitch;

	const glm::vec3 m_UpDir = glm::vec3(0.0f, 1.0f, 0.0f);
};