#pragma once

#include "Chunk.h"

class ChunkColumn {
    public:
	ChunkColumn(const glm::ivec2 &position);

	const glm::ivec2 &GetPosition() const
	{
		return m_Position;
	}

	void SetNeighborChunks(ChunkColumn *left, ChunkColumn *right,
			       ChunkColumn *back, ChunkColumn *front);

	void SetBlock(const glm::ivec3 &position, int type);
	// Sets the layers in the column to <type> from lower to upper inclusive
	void SetLayers(uint32_t lower, uint32_t upper, int type);
	void SetAllBlocks(int type);

	void BuildAllMeshes();

	std::array<std::unique_ptr<Chunk>, 16> &GetChunks();

	bool HasAllNeighbors();

	void SetDoDraw(bool doDraw)
	{
		m_DoDraw = doDraw;
	}
	const bool &GetDoDraw()
	{
		return m_DoDraw;
	}
	void Render();

	const uint32_t &GetMemoryUsage();

    private:
	void CalculateMemoryUsage();

    private:
	glm::ivec2 m_Position;

	std::array<std::unique_ptr<Chunk>, 16> m_Chunks;

	std::array<ChunkColumn *, 4> m_NeighborColumns;

	bool m_DoDraw = true;
	std::unique_ptr<VertexArray> m_SolidMesh;

	uint32_t m_ColumnMemoryUsage = 0;
};