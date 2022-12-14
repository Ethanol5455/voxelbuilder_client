#pragma once

class RayCast
{
public:
	RayCast(const glm::vec3& direction, float stepSize, float rayLength);

	bool Step();
	bool StepBack();

	const glm::vec3& GetPositionOffset() { return m_Position; }
	const glm::vec3& GetDirection() { return m_Direction; }
	const float& GetStepSize() { return m_StepSize; }
	const float& GetRayLength() { return m_RayLength; }
private:
	glm::vec3 m_Position;
	glm::vec3 m_Direction;
	float m_StepSize;
	float m_RayLength;

	float m_CurrentSteps;
};