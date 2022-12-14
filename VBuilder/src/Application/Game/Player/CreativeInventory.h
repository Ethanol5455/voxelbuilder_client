#pragma once

#include "Renderer/WorldRenderer.h"

struct BlockInfo
{
	int ID;
	std::string name;
	glm::vec2 topTexPos = glm::vec2(0, 9);
	glm::vec2 sideTexPos = glm::vec2(0, 9);
	glm::vec2 bottomTexPos = glm::vec2(0, 9);
};

class CreativeInventory
{
public:
	CreativeInventory();

	void Display();

	std::vector<BlockInfo>& GetStacks() { return m_Stacks; }
	const BlockInfo& GetBlockInfo(uint32_t i) { return m_Stacks[i]; }
	size_t GetNumStacks() { return m_Stacks.size(); }

private:
	std::vector<BlockInfo> m_Stacks;
};