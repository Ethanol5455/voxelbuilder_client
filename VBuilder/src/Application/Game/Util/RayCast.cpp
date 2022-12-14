#include "vbpch.h"
#include "RayCast.h"

RayCast::RayCast(const glm::vec3& direction, float stepSize, float rayLength)
	: m_Position(glm::vec3(0.0f)), m_Direction(direction), m_StepSize(stepSize), m_RayLength(rayLength), m_CurrentSteps(0)
{
}

bool RayCast::Step()
{
	float numRaySteps = m_RayLength / m_StepSize;
	if (m_CurrentSteps <= numRaySteps)
	{
		m_Position = glm::vec3(m_Direction.x * m_CurrentSteps * m_StepSize,
								m_Direction.y * m_CurrentSteps * m_StepSize,
								m_Direction.z * m_CurrentSteps * m_StepSize);
		m_CurrentSteps++;
		return true;
	}
	return false;
}

bool RayCast::StepBack()
{
	float numRaySteps = m_RayLength / m_StepSize;
	if (m_CurrentSteps >= 0)
	{
		m_Position = glm::vec3(m_Direction.x * m_CurrentSteps * m_StepSize,
			m_Direction.y * m_CurrentSteps * m_StepSize,
			m_Direction.z * m_CurrentSteps * m_StepSize);
		m_CurrentSteps--;
		return true;
	}
	return false;
}
