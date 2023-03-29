#pragma once

#include "Core/Input.h"

#include "FastNoiseLite.h"

#include "ChunkColumn.h"

#include "WorldUtils.h"
#include "Application/Game/Networking/Packets.h"

class Map {
    public:
	Map();
	~Map();

	void FetchColumnFromServer(ENetPeer *server, int columnXPos,
				   int columnZPos);

	void IntegrateColumnFromServer(uint8_t *data, size_t dataLength);

	void SetAllNeighbors(int columnXPos, int columnZPos);
	void BuildSurroundingMeshes(int columnXPos, int columnZPos);

	void Update(ENetPeer *server, const glm::vec3 &cameraPosition,
		    float renderDistance);
	void Render();

	int GetBlock(glm::ivec3 position);
	void SetBlock(glm::ivec3 position, int type, bool buildMeshes = false);

	ChunkColumn *GetChunkColumnAt(int columnXPos, int columnZPos);

	static glm::ivec2 WorldToColumnPosition(const glm::ivec2 &worldPos);
	static glm::ivec3 WorldToChunkPosition(const glm::ivec3 &worldPos);
	static glm::ivec3
	ColumnToWorldPosition(const glm::ivec2 &columnPosition,
			      const glm::ivec3 &blockPosition);
	static glm::ivec3
	WorldToPositionInColumn(const glm::ivec3 &worldPosition);
	static glm::ivec3
	WorldToPositionInChunk(const glm::ivec3 &worldPosition);

	uint32_t GetMemoryUsage();

    private:
	// x,z
	std::map<int, std::map<int, std::shared_ptr<ChunkColumn> > > m_ColumnMap;

	std::vector<glm::ivec2> m_InFlight;
};