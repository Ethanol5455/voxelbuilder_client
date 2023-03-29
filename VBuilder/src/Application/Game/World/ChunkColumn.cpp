#include "vbpch.h"

#include "ChunkColumn.h"

ChunkColumn::ChunkColumn(const glm::ivec2 &position)
	: m_Position(position)
{
	for (int i = 0; i < 16; i++) {
		m_Chunks[i] = std::make_unique<Chunk>(
			glm::ivec3(position.x, i, position.y));
	}
}

void ChunkColumn::SetNeighborChunks(ChunkColumn *left, ChunkColumn *right,
				    ChunkColumn *back, ChunkColumn *front)
{
	m_NeighborColumns[Directions::Left] = left;
	m_NeighborColumns[Directions::Right] = right;
	m_NeighborColumns[Directions::Back] = back;
	m_NeighborColumns[Directions::Front] = front;

	{
		Chunk *leftChunk = nullptr;
		if (m_NeighborColumns[Directions::Left] != nullptr)
			leftChunk = &(*m_NeighborColumns[Directions::Left]
					       ->GetChunks()[0]);
		Chunk *rightChunk = nullptr;
		if (m_NeighborColumns[Directions::Right] != nullptr)
			rightChunk = &(*m_NeighborColumns[Directions::Right]
						->GetChunks()[0]);
		Chunk *backChunk = nullptr;
		if (m_NeighborColumns[Directions::Back] != nullptr)
			backChunk = &(*m_NeighborColumns[Directions::Back]
					       ->GetChunks()[0]);
		Chunk *frontChunk = nullptr;
		if (m_NeighborColumns[Directions::Front] != nullptr)
			frontChunk = &(*m_NeighborColumns[Directions::Front]
						->GetChunks()[0]);
		m_Chunks[0]->SetNeighborChunks(&(*m_Chunks[1]), nullptr,
					       leftChunk, rightChunk, backChunk,
					       frontChunk);
	}
	{
		Chunk *leftChunk = nullptr;
		if (m_NeighborColumns[Directions::Left] != nullptr)
			leftChunk = &(*m_NeighborColumns[Directions::Left]
					       ->GetChunks()[15]);
		Chunk *rightChunk = nullptr;
		if (m_NeighborColumns[Directions::Right] != nullptr)
			rightChunk = &(*m_NeighborColumns[Directions::Right]
						->GetChunks()[15]);
		Chunk *backChunk = nullptr;
		if (m_NeighborColumns[Directions::Back] != nullptr)
			backChunk = &(*m_NeighborColumns[Directions::Back]
					       ->GetChunks()[15]);
		Chunk *frontChunk = nullptr;
		if (m_NeighborColumns[Directions::Front] != nullptr)
			frontChunk = &(*m_NeighborColumns[Directions::Front]
						->GetChunks()[15]);
		m_Chunks[15]->SetNeighborChunks(nullptr, &(*m_Chunks[14]),
						leftChunk, rightChunk,
						backChunk, frontChunk);
	}

	for (uint32_t i = 1; i < 15; i++) {
		Chunk *leftChunk = nullptr;
		if (m_NeighborColumns[Directions::Left] != nullptr)
			leftChunk = &(*m_NeighborColumns[Directions::Left]
					       ->GetChunks()[i]);
		Chunk *rightChunk = nullptr;
		if (m_NeighborColumns[Directions::Right] != nullptr)
			rightChunk = &(*m_NeighborColumns[Directions::Right]
						->GetChunks()[i]);
		Chunk *backChunk = nullptr;
		if (m_NeighborColumns[Directions::Back] != nullptr)
			backChunk = &(*m_NeighborColumns[Directions::Back]
					       ->GetChunks()[i]);
		Chunk *frontChunk = nullptr;
		if (m_NeighborColumns[Directions::Front] != nullptr)
			frontChunk = &(*m_NeighborColumns[Directions::Front]
						->GetChunks()[i]);
		m_Chunks[i]->SetNeighborChunks(&(*m_Chunks[i + 1]),
					       &(*m_Chunks[i - 1]), leftChunk,
					       rightChunk, backChunk,
					       frontChunk);
	}
}

void ChunkColumn::CalculateMemoryUsage()
{
	m_ColumnMemoryUsage =
		16 * sizeof(int) * 4096 +
		(m_SolidMesh->GetIndexCount() * 4 / 6) * sizeof(QuadVertex);
}

void ChunkColumn::BuildAllMeshes()
{
	for (auto &chunk : m_Chunks) {
		if (!chunk->IsMeshUpToDate())
			chunk->BuildMesh();
	}

	uint32_t numVertices = 0;

	for (auto &chunk : m_Chunks) {
		numVertices += chunk->GetSolidMesh().numVerticies;
	}

	QuadVertex *vertices = new QuadVertex[numVertices];
	{
		uint32_t offset = 0;
		for (auto &chunk : m_Chunks) {
			const Mesh &mesh = chunk->GetSolidMesh();
			memcpy(vertices + offset, mesh.verticies,
			       mesh.numVerticies * sizeof(QuadVertex));
			offset += mesh.numVerticies;
		}
	}

	{
		VertexBufferLayout layout;
		layout.Push<float>(3);
		layout.Push<float>(2);

		uint32_t *quadIndices = new uint32_t[numVertices * 6 / 4];

		uint32_t offset = 0;
		for (uint32_t i = 0; i < numVertices * 6 / 4; i += 6) {
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;

			offset += 4;
		}

		m_SolidMesh = std::make_unique<VertexArray>(
			&vertices->Position.x,
			numVertices * (uint32_t)sizeof(QuadVertex), layout,
			quadIndices, numVertices * 6 / 4);

		delete[] quadIndices;
	}

	delete[] vertices;

	CalculateMemoryUsage();
}

void ChunkColumn::SetBlock(const glm::ivec3 &position, int type)
{
	m_Chunks[position.y / 16]->SetBlock(
		{ position.x, position.y % 16, position.z }, type);
}

void ChunkColumn::SetLayers(uint32_t lower, uint32_t upper, int type)
{
	for (uint32_t y = lower; y <= upper; y++) {
		for (uint32_t x = 0; x <= 15; x++) {
			for (uint32_t z = 0; z <= 15; z++) {
				SetBlock({ x, y, z }, type);
			}
		}
	}
}

void ChunkColumn::SetAllBlocks(int type)
{
	for (int i = 0; i < 16; i++) {
		m_Chunks[i]->SetAll(type);
	}
}

std::array<std::unique_ptr<Chunk>, 16> &ChunkColumn::GetChunks()
{
	return m_Chunks;
}

bool ChunkColumn::HasAllNeighbors()
{
	return m_NeighborColumns[0] && m_NeighborColumns[1] &&
	       m_NeighborColumns[2] && m_NeighborColumns[3];
}

void ChunkColumn::Render()
{
	WorldRenderer::DrawColumnVA(m_SolidMesh.get(), glm::vec2(0.0f));
}

const uint32_t &ChunkColumn::GetMemoryUsage()
{
	return m_ColumnMemoryUsage;
}
