#pragma once

#include "Renderer/WorldRenderer.h"

#include "Application/Game/Util/AssetInfo.h"

enum Directions { Front, Back, Left, Right, Top, Bottom };

class Chunk {
    public:
	Chunk(const glm::ivec3 &position);
	~Chunk();

	void SetNeighborChunks(Chunk *top, Chunk *bottom, Chunk *left,
			       Chunk *right, Chunk *back, Chunk *front);

	void SetBlock(const glm::ivec3 &position, int type);
	void SetBlock(const int &i, int type);
	int GetBlock(const glm::ivec3 &position);
	int GetBlock(const int &i) const;
	void SetAll(int type);

	void BuildMesh();
	const bool &IsMeshUpToDate()
	{
		return m_IsMeshUpToDate;
	}
	const Mesh &GetSolidMesh() const
	{
		return m_SolidMesh;
	}

	const glm::ivec3 &GetPosition() const
	{
		return m_Position;
	}

    private:
	bool IsTransparent(int id);

    private:
	glm::ivec3 m_Position;

	int *m_Blocks;

	bool m_IsMeshUpToDate = false;
	Mesh m_SolidMesh;

	std::array<Chunk *, 6> m_NeighborChunks;
};