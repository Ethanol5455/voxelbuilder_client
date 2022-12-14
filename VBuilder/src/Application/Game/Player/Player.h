#pragma once

#include "Camera3D.h"
#include "CreativeInventory.h"
#include "Application/Game/Util/AssetInfo.h"

class Player
{
public:
	Player();

	void Update(float deltaTime);

	void SetUsername(const std::string& username) { m_Username = username; }
	const std::string& GetUsername() { return m_Username; }

	void SetInventorySelection(int i) { m_Selection = i; }
	int GetInventorySelection() { return m_Selection; }

	const glm::vec3 GetPosition() const { return m_Camera->GetPosition(); }
	void SetPosition(const glm::vec3& position) { m_Camera->SetPosition(position); }
	void Move(const glm::vec3& distance) { m_Camera->Move(distance); }

	CreativeInventory& GetInventory() { return m_CreativeInventory; }

	Camera3D& GetCamera() { return *m_Camera; }

private:
	std::string m_Username;

	std::unique_ptr<Camera3D> m_Camera;

	CreativeInventory m_CreativeInventory;
	int m_Selection;
};