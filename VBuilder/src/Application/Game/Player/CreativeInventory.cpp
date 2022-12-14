#include "vbpch.h"

#include "CreativeInventory.h"

CreativeInventory::CreativeInventory()
{
	m_Stacks.emplace_back(BlockInfo{ 1, "Dirt",
		glm::vec2(1, 9), glm::vec2(1, 9), glm::vec2(1, 9) });
	m_Stacks.emplace_back(BlockInfo{ 2, "Grass",
		glm::vec2(3, 9), glm::vec2(2, 9), glm::vec2(1, 9) });
	m_Stacks.emplace_back(BlockInfo{ 3, "Stone",
		glm::vec2(4, 9), glm::vec2(4, 9), glm::vec2(4, 9) });
	m_Stacks.emplace_back(BlockInfo{ 4, "Cobblestone",
		glm::vec2(5, 9), glm::vec2(5, 9), glm::vec2(5, 9) });
}

void CreativeInventory::Display()
{
	//Renderer::DrawMeshOfQuads
}
