#include "vbpch.h"

#include "Chunk.h"

uint32_t xyzToI(const uint32_t& x, const uint32_t& y, const uint32_t& z)
{
	return (uint32_t)(256 * z + 16 * y + x);
}

static int s_AirID = 0;

Chunk::Chunk(const glm::ivec3& position)
	: m_Position(position)
{
	s_AirID = ItemInfoManager::GetIDByName("Air");

	m_Blocks = new int[4096];

	std::fill(m_Blocks, m_Blocks + 4096, s_AirID);

	for (int i = 0; i < 6; i++)
		m_NeighborChunks[i] = nullptr;
}

Chunk::~Chunk()
{
	delete[] m_Blocks;

	//if (m_SolidMesh.verticies)
	//	delete[] m_SolidMesh.verticies;
}

void Chunk::SetNeighborChunks(Chunk* top, Chunk* bottom, Chunk* left, Chunk* right, Chunk* back, Chunk* front)
{
	m_NeighborChunks[Directions::Left] = left;
	m_NeighborChunks[Directions::Right] = right;
	m_NeighborChunks[Directions::Back] = back;
	m_NeighborChunks[Directions::Front] = front;
	m_NeighborChunks[Directions::Top] = top;
	m_NeighborChunks[Directions::Bottom] = bottom;
	m_IsMeshUpToDate = false;
}

void Chunk::SetBlock(const glm::ivec3& position, int type)
{
	m_Blocks[xyzToI((uint32_t)position.x, (uint32_t)position.y, (uint32_t)position.z)] = type;
	m_IsMeshUpToDate = false;
}

void Chunk::SetBlock(const int& i, int type)
{
	m_Blocks[i] = type;
	m_IsMeshUpToDate = false;
}

int Chunk::GetBlock(const glm::ivec3& position)
{
	return m_Blocks[xyzToI((uint32_t)position.x, (uint32_t)position.y, (uint32_t)position.z)];
}

int Chunk::GetBlock(const int& i) const
{
	return m_Blocks[i];
}

void Chunk::SetAll(int type)
{
	std::fill(m_Blocks, m_Blocks + 4096, type);
	m_IsMeshUpToDate = false;
}

void Chunk::BuildMesh()
{
	std::vector<QuadVertex> verticies;

	for (int z = 0; z < 16; z++)
	{
		for (int y = 0; y < 16; y++)
		{
			for (int x = 0; x < 16; x++)
			{
				uint32_t i = xyzToI(x, y, z);
				int& thisBlockID = m_Blocks[i];
				const ItemInfo* thisBlockInfo = ItemInfoManager::GetItemByID(thisBlockID);
				if (thisBlockID == s_AirID || thisBlockID == -1)
				{
					continue;
				}
				else // Block is not air:
				{
					if (thisBlockInfo->Type == ItemType::BlockCube) // Standard cube pattern
					{
						bool drawFrontFace = true;
						bool drawBackFace = true;
						bool drawLeftFace = true;
						bool drawRightFace = true;
						bool drawBottomFace = true;
						bool drawTopFace = true;
						
						// Right Face
						if (z == 15 && m_NeighborChunks[Directions::Right])
						{
							drawRightFace = IsTransparent(m_NeighborChunks[Directions::Right]->GetBlock({ x, y, 0 }));
						}
						else if (z != 15)
						{
							drawRightFace = IsTransparent(m_Blocks[xyzToI(x, y, z + 1)]);
						}
						if (drawRightFace)
						{
							QuadVertex bl;
							bl.Position = { x + m_Position.x * 16, y + m_Position.y * 16, z + 1.0f + m_Position.z * 16 };
							bl.TexCoord = thisBlockInfo->SideTexCoords.BL;

							QuadVertex br;
							br.Position = { x + 1.0f + m_Position.x * 16, y + m_Position.y * 16, z + 1.0f + m_Position.z * 16 };
							br.TexCoord = thisBlockInfo->SideTexCoords.BR;

							QuadVertex tr;
							tr.Position = { x + 1.0f + m_Position.x * 16, y + 1.0f + m_Position.y * 16, z + 1.0f + m_Position.z * 16 };
							tr.TexCoord = thisBlockInfo->SideTexCoords.TR;

							QuadVertex tl;
							tl.Position = { x + m_Position.x * 16, y + 1.0f + m_Position.y * 16, z + 1.0f + m_Position.z * 16 };
							tl.TexCoord = thisBlockInfo->SideTexCoords.TL;

							verticies.push_back(bl);
							verticies.push_back(br);
							verticies.push_back(tr);
							verticies.push_back(tl);
						}
						
						if (z == 0 && m_NeighborChunks[Directions::Left])
						{
							drawLeftFace = IsTransparent(m_NeighborChunks[Directions::Left]->GetBlock({ x, y, 15 }));
						}
						else if (z != 0)
						{
							drawLeftFace = IsTransparent(m_Blocks[xyzToI(x, y, z - 1)]);
						}
						// Left Face
						if (drawLeftFace)
						{
							QuadVertex bl;
							bl.Position = { x + m_Position.x * 16, y + m_Position.y * 16, z + m_Position.z * 16 };
							bl.TexCoord = thisBlockInfo->SideTexCoords.BL;

							QuadVertex br;
							br.Position = { x + 1.0f + m_Position.x * 16, y + m_Position.y * 16, z + m_Position.z * 16 };
							br.TexCoord = thisBlockInfo->SideTexCoords.BR;

							QuadVertex tr;
							tr.Position = { x + 1.0f + m_Position.x * 16, y + 1.0f + m_Position.y * 16, z + m_Position.z * 16 };
							tr.TexCoord = thisBlockInfo->SideTexCoords.TR;

							QuadVertex tl;
							tl.Position = { x + m_Position.x * 16, y + 1.0f + m_Position.y * 16, z + m_Position.z * 16 };
							tl.TexCoord = thisBlockInfo->SideTexCoords.TL;

							verticies.push_back(bl);
							verticies.push_back(tl);
							verticies.push_back(tr);
							verticies.push_back(br);
						}
						

						if (x == 0 && m_NeighborChunks[Directions::Front])
						{
							drawFrontFace = IsTransparent(m_NeighborChunks[Directions::Front]->GetBlock({ 15, y, z }));
						}
						else if (x != 0)
						{
							drawFrontFace = IsTransparent(m_Blocks[xyzToI(x - 1, y, z)]);
						}
						// Front Face
						if (drawFrontFace)
						{
							QuadVertex bl;
							bl.Position = { x + m_Position.x * 16, y + m_Position.y * 16, z + m_Position.z * 16 };
							bl.TexCoord = thisBlockInfo->SideTexCoords.BL;

							QuadVertex br;
							br.Position = { x + m_Position.x * 16, y + m_Position.y * 16, z + 1.0f + m_Position.z * 16 };
							br.TexCoord = thisBlockInfo->SideTexCoords.BR;

							QuadVertex tr;
							tr.Position = { x + m_Position.x * 16, y + 1.0f + m_Position.y * 16, z + 1.0f + m_Position.z * 16 };
							tr.TexCoord = thisBlockInfo->SideTexCoords.TR;

							QuadVertex tl;
							tl.Position = { x + m_Position.x * 16, y + 1.0f + m_Position.y * 16, z + m_Position.z * 16 };
							tl.TexCoord = thisBlockInfo->SideTexCoords.TL;

							verticies.push_back(bl);
							verticies.push_back(br);
							verticies.push_back(tr);
							verticies.push_back(tl);
						}

						if (x == 15 && m_NeighborChunks[Directions::Back])
						{
							drawBackFace = IsTransparent(m_NeighborChunks[Directions::Back]->GetBlock({ 0, y, z }));
						}
						else if (x != 15)
						{
							drawBackFace = IsTransparent(m_Blocks[xyzToI(x + 1, y, z)]);
						}
						// Back Face
						if (drawBackFace)
						{
							QuadVertex bl;
							bl.Position = { x + 1.0f + m_Position.x * 16, y + m_Position.y * 16, z + m_Position.z * 16 };
							bl.TexCoord = thisBlockInfo->SideTexCoords.BL;

							QuadVertex br;
							br.Position = { x + 1.0f + m_Position.x * 16, y + m_Position.y * 16, z + 1.0f + m_Position.z * 16 };
							br.TexCoord = thisBlockInfo->SideTexCoords.BR;

							QuadVertex tr;
							tr.Position = { x + 1.0f + m_Position.x * 16, y + 1.0f + m_Position.y * 16, z + 1.0f + m_Position.z * 16 };
							tr.TexCoord = thisBlockInfo->SideTexCoords.TR;

							QuadVertex tl;
							tl.Position = { x + 1.0f + m_Position.x * 16, y + 1.0f + m_Position.y * 16, z + m_Position.z * 16 };
							tl.TexCoord = thisBlockInfo->SideTexCoords.TL;

							verticies.push_back(bl);
							verticies.push_back(tl);
							verticies.push_back(tr);
							verticies.push_back(br);
						}
						
						if (y == 0 && m_NeighborChunks[Directions::Bottom])
						{
							drawBottomFace = IsTransparent(m_NeighborChunks[Directions::Bottom]->GetBlock({ x, 15, z }));
						}
						else if (y != 0)
						{
							drawBottomFace = IsTransparent(m_Blocks[xyzToI(x, y - 1, z)]);
						}
						//Bottom Face
						if (drawBottomFace)
						{
							QuadVertex bl;
							bl.Position = { x + m_Position.x * 16, y + m_Position.y * 16, z + 1.0f + m_Position.z * 16 };
							bl.TexCoord = thisBlockInfo->BottomTexCoords.BL;

							QuadVertex br;
							br.Position = { x + 1.0f + m_Position.x * 16, y + m_Position.y * 16, z + 1.0f + m_Position.z * 16 };
							br.TexCoord = thisBlockInfo->BottomTexCoords.BR;

							QuadVertex tr;
							tr.Position = { x + 1.0f + m_Position.x * 16, y + m_Position.y * 16, z + m_Position.z * 16 };
							tr.TexCoord = thisBlockInfo->BottomTexCoords.TR;

							QuadVertex tl;
							tl.Position = { x + m_Position.x * 16, y + m_Position.y * 16, z + m_Position.z * 16 };
							tl.TexCoord = thisBlockInfo->BottomTexCoords.TL;

							verticies.push_back(bl);
							verticies.push_back(tl);
							verticies.push_back(tr);
							verticies.push_back(br);
						}

						if (y == 15 && m_NeighborChunks[Directions::Top])
						{
							drawTopFace = IsTransparent(m_NeighborChunks[Directions::Top]->GetBlock({ x, 0, z }));
						}
						else if (y != 15)
						{
							drawTopFace = IsTransparent(m_Blocks[xyzToI(x, y + 1, z)]);
						}
						//Top Face
						if (drawTopFace)
						{
							QuadVertex bl;
							bl.Position = { x + m_Position.x * 16, y + 1.0f + m_Position.y * 16, z + 1.0f + m_Position.z * 16 };
							bl.TexCoord = thisBlockInfo->TopTexCoords.BL;

							QuadVertex br;
							br.Position = { x + 1.0f + m_Position.x * 16, y + 1.0f + m_Position.y * 16, z + 1.0f + m_Position.z * 16 };
							br.TexCoord = thisBlockInfo->TopTexCoords.BR;

							QuadVertex tr;
							tr.Position = { x + 1.0f + m_Position.x * 16, y + 1.0f + m_Position.y * 16, z + m_Position.z * 16 };
							tr.TexCoord = thisBlockInfo->TopTexCoords.TR;

							QuadVertex tl;
							tl.Position = { x + m_Position.x * 16, y + 1.0f + m_Position.y * 16, z + m_Position.z * 16 };
							tl.TexCoord = thisBlockInfo->TopTexCoords.TL;

							verticies.push_back(bl);
							verticies.push_back(br);
							verticies.push_back(tr);
							verticies.push_back(tl);
						}
					}
					else if (thisBlockInfo->Type == ItemType::BlockCross)// Block is a cross pattern
					{
						{
							QuadVertex bl;
							bl.Position = { x + m_Position.x * 16, y + m_Position.y * 16, z + m_Position.z * 16 };
							bl.TexCoord = thisBlockInfo->SideTexCoords.BL;

							QuadVertex br;
							br.Position = { x + 1.0f + m_Position.x * 16, y + m_Position.y * 16, z + 1.0f + m_Position.z * 16 };
							br.TexCoord = thisBlockInfo->SideTexCoords.BR;

							QuadVertex tr;
							tr.Position = { x + 1.0f + m_Position.x * 16, y + 1.0f + m_Position.y * 16, z + 1.0f + m_Position.z * 16 };
							tr.TexCoord = thisBlockInfo->SideTexCoords.TR;

							QuadVertex tl;
							tl.Position = { x + m_Position.x * 16, y + 1.0f + m_Position.y * 16, z + m_Position.z * 16 };
							tl.TexCoord = thisBlockInfo->SideTexCoords.TL;

							verticies.push_back(bl);
							verticies.push_back(br);
							verticies.push_back(tr);
							verticies.push_back(tl);

							verticies.push_back(tl);
							verticies.push_back(tr);
							verticies.push_back(br);
							verticies.push_back(bl);
						}

						{
							QuadVertex bl;
							bl.Position = { x + m_Position.x * 16, y + m_Position.y * 16, z + 1.0f + m_Position.z * 16 };
							bl.TexCoord = thisBlockInfo->SideTexCoords.BL;

							QuadVertex br;
							br.Position = { x + 1.0f + m_Position.x * 16, y + m_Position.y * 16, z + m_Position.z * 16 };
							br.TexCoord = thisBlockInfo->SideTexCoords.BR;

							QuadVertex tr;
							tr.Position = { x + 1.0f + m_Position.x * 16, y + 1.0f + m_Position.y * 16, z + m_Position.z * 16 };
							tr.TexCoord = thisBlockInfo->SideTexCoords.TR;

							QuadVertex tl;
							tl.Position = { x + m_Position.x * 16, y + 1.0f + m_Position.y * 16, z + 1.0f + m_Position.z * 16 };
							tl.TexCoord = thisBlockInfo->SideTexCoords.TL;

							verticies.push_back(bl);
							verticies.push_back(br);
							verticies.push_back(tr);
							verticies.push_back(tl);

							verticies.push_back(tl);
							verticies.push_back(tr);
							verticies.push_back(br);
							verticies.push_back(bl);
						}
					}
				}
			}
		}
	}

	if (m_SolidMesh.verticies != nullptr)
		delete[] m_SolidMesh.verticies;
	
	m_SolidMesh.numVerticies = (uint32_t)verticies.size();
	m_SolidMesh.verticies = new QuadVertex[(uint32_t)verticies.size()];
	
	for (size_t i = 0; i < verticies.size(); i++)
	{
		m_SolidMesh.verticies[i].Position = verticies.at(i).Position;
		m_SolidMesh.verticies[i].TexCoord = verticies.at(i).TexCoord;
	}
}

bool Chunk::IsTransparent(int id)
{
	if (id == s_AirID || ItemInfoManager::GetItemByID(id)->Type == ItemType::BlockCross || ItemInfoManager::GetItemByID(id)->IsTransparent)
		return true;
	return false;
}
