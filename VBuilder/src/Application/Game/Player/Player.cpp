#include "vbpch.h"

#include "Player.h"

Player::Player()
{
	m_Selection = ItemInfoManager::GetIDByName("Dirt");
	m_Camera = std::make_unique<Camera3D>(glm::vec3(0.0f, 0.0f, 0.0f));
}

void Player::Update(float deltaTime)
{
	m_Camera->Update(deltaTime);
}
