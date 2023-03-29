#include "vbpch.h"

#include "Map.h"

Map::Map()
{
}

Map::~Map()
{
}

void Map::FetchColumnFromServer(ENetPeer *server, int columnXPos,
				int columnZPos)
{
	for (glm::ivec2 &inFlight : m_InFlight) {
		if (inFlight.x == columnXPos && inFlight.y == columnZPos)
			return;
	}

	// Request Column from server
	std::vector<uint8_t> packetData =
		AssembleChunkRequestPacket(columnXPos, columnZPos);
	ENetPacket *packet = enet_packet_create(
		&packetData[0], packetData.size(), ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(server, 0, packet);

	m_InFlight.push_back(glm::ivec2{ columnXPos, columnZPos });
}

void Map::IntegrateColumnFromServer(uint8_t *data, size_t dataLength)
{
	if (data[0] != PacketType::ChunkContents) {
		VB_WARN("Attempted to integrate column not of PacketType ChunkContents");
		return;
	}

	glm::ivec3 columnPos;

	for (int i = 1; i < dataLength; i++) {
		columnPos = BytesToNum<glm::ivec3>(data + i);
		i += 12;

		ChunkColumn *column =
			GetChunkColumnAt(columnPos.x, columnPos.z);
		if (column == nullptr) {
			std::shared_ptr<ChunkColumn> thisColumn =
				std::make_shared<ChunkColumn>(
					glm::ivec2(columnPos.x, columnPos.z));
			m_ColumnMap[columnPos.x][columnPos.z] = thisColumn;
			column = thisColumn.get();
		}

		std::shared_ptr<std::vector<CompressedSet> > compressedData =
			std::make_shared<std::vector<CompressedSet> >();

		while (BytesToNum<int32_t>(data + i) != -1) {
			int32_t type = BytesToNum<int32_t>(data + i);
			i += 4;
			int32_t number = BytesToNum<int32_t>(data + i);
			i += 4;
			compressedData->emplace_back(
				CompressedSet{ type, number });
		}

		i += 3;
		//VB_TRACE("{0}", i);

		Chunk *chunk = column->GetChunks()[columnPos.y].get();
		int index = 0;
		for (int a = 0; a < compressedData->size(); a++) {
			int num = compressedData->at(a).number;
			for (int q = 0; q < num; q++) {
				chunk->SetBlock(index,
						compressedData->at(a).type);
				index++;
			}
		}
	}

	for (int i = 0; i < m_InFlight.size(); i++) {
		glm::ivec2 &inFlight = m_InFlight[i];
		if (inFlight.x == columnPos.x && inFlight.y == columnPos.z) {
			m_InFlight.erase(m_InFlight.begin() + i);
			i--;
		}
	}

	BuildSurroundingMeshes(columnPos.x, columnPos.z);
}

void Map::SetAllNeighbors(int columnXPos, int columnZPos)
{
	ChunkColumn *f = nullptr;
	ChunkColumn *b = nullptr;
	ChunkColumn *l = nullptr;
	ChunkColumn *r = nullptr;
	auto &xP1 = m_ColumnMap[columnXPos + 1];
	if (xP1.count(columnZPos))
		b = xP1[columnZPos].get();
	auto &xM1 = m_ColumnMap[columnXPos - 1];
	if (xM1.count(columnZPos))
		f = xM1[columnZPos].get();
	auto &x0 = m_ColumnMap[columnXPos];
	if (x0.count(columnZPos - 1))
		l = x0[columnZPos - 1].get();
	if (x0.count(columnZPos + 1))
		r = x0[columnZPos + 1].get();
	m_ColumnMap[columnXPos][columnZPos]->SetNeighborChunks(l, r, b, f);
}

void Map::BuildSurroundingMeshes(int columnXPos, int columnZPos)
{
	const glm::ivec2 positions[] = { { columnXPos + 1, columnZPos },
					 { columnXPos - 1, columnZPos },
					 { columnXPos, columnZPos - 1 },
					 { columnXPos, columnZPos + 1 },
					 { columnXPos, columnZPos } };

	for (auto &position : positions) {
		ChunkColumn *column = GetChunkColumnAt(position.x, position.y);
		if (column) {
			SetAllNeighbors(position.x, position.y);
			column->BuildAllMeshes();
		}
	}
}

void Map::Update(ENetPeer *server, const glm::vec3 &cameraPosition,
		 float renderDistance)
{
	glm::ivec2 cameraColumnPos = WorldToColumnPosition(
		glm::ivec2(cameraPosition.x, cameraPosition.z));
	{
		int spiralTarget = int(renderDistance / 16.0f);
		int spiralRadius = 0;
		glm::ivec2 spiralDir = glm::ivec2(1, 0);
		glm::ivec2 columnPos = cameraColumnPos;

		bool doGenerate = true;
		while (doGenerate) {
			ChunkColumn *column =
				GetChunkColumnAt(columnPos.x, columnPos.y);
			if (column == nullptr) {
				FetchColumnFromServer(server, (int)columnPos.x,
						      (int)columnPos.y);
				doGenerate = false;
			} else {
				glm::vec2 positionDifference = glm::vec2(
					cameraPosition.x - columnPos.x * 16.0f +
						8.0f,
					cameraPosition.z - columnPos.y * 16.0f +
						8.0f);
				float distanceToChunk = (float)glm::sqrt(
					positionDifference.x *
						positionDifference.x +
					positionDifference.y *
						positionDifference.y);
				if (spiralRadius >= spiralTarget ||
				    distanceToChunk > renderDistance * 1.1f) {
					doGenerate = false;
					continue;
				}

				glm::ivec2 diff = columnPos - cameraColumnPos;
				if (diff.x == spiralRadius &&
				    columnPos.y == cameraColumnPos.y) {
					columnPos += glm::ivec2(1, -1);
					spiralDir = glm::ivec2(0, -1);
					spiralRadius++;
					if (spiralRadius >= spiralTarget)
						doGenerate = false;
					continue;
				}
				if (diff.y == -spiralRadius &&
				    diff.x == spiralRadius) {
					spiralDir = glm::ivec2(-1, 0);
				}
				if (diff.y == -spiralRadius &&
				    diff.x == -spiralRadius) {
					spiralDir = glm::ivec2(0, 1);
				}
				if (diff.y == spiralRadius &&
				    diff.x == -spiralRadius) {
					spiralDir = glm::ivec2(1, 0);
				}
				if (diff.y == spiralRadius &&
				    diff.x == spiralRadius) {
					spiralDir = glm::ivec2(0, -1);
				}

				columnPos += spiralDir;
			}
		}
	}

	for (auto &columnX : m_ColumnMap) {
		for (auto &columnZ : columnX.second) {
			glm::vec2 positionDifference = glm::vec2(
				cameraPosition.x - columnX.first * 16.0f + 8.0f,
				cameraPosition.z - columnZ.first * 16.0f +
					8.0f);
			float distanceToChunk = (float)glm::sqrt(
				positionDifference.x * positionDifference.x +
				positionDifference.y * positionDifference.y);
			if (distanceToChunk <= renderDistance * 1.2f) {
				columnZ.second->SetDoDraw(distanceToChunk <=
							  renderDistance);
			} else {
				columnZ.second->SetDoDraw(false);
				const auto &it =
					columnX.second.find(columnZ.first);
				columnX.second.erase(it);
				break;
			}
		}
	}
}

void Map::Render()
{
	for (auto &columnX : m_ColumnMap) {
		for (auto &columnZ : columnX.second) {
			if (columnZ.second->GetDoDraw())
				columnZ.second->Render();
		}
	}
}

int Map::GetBlock(glm::ivec3 position)
{
	glm::ivec3 chunkPosition = WorldToChunkPosition(position);
	glm::ivec3 blockPositionInChunk = WorldToPositionInChunk(position);

	auto *column =
		GetChunkColumnAt((int)chunkPosition.x, (int)chunkPosition.z);
	if (column) {
		if (chunkPosition.y >= 0 && chunkPosition.y <= 15) {
			auto *chunk =
				&*column->GetChunks()[(int)chunkPosition.y];
			if (chunk != nullptr) {
				return chunk->GetBlock(blockPositionInChunk);
			}
		}
	}
	return -1;
}

void Map::SetBlock(glm::ivec3 position, int type, bool buildMeshes)
{
	if (!ItemInfoManager::GetItemByID(type)) {
		VB_ERROR("Tried to set block of unknown id {0}", type);
		return;
	}

	glm::ivec2 columnPosition =
		WorldToColumnPosition(glm::ivec2(position.x, position.z));
	glm::ivec3 blockPositionInColumn = WorldToPositionInColumn(position);

	auto *column =
		GetChunkColumnAt((int)columnPosition.x, (int)columnPosition.y);
	if (column) {
		column->SetBlock(blockPositionInColumn, type);

		if (buildMeshes)
			BuildSurroundingMeshes(columnPosition.x,
					       columnPosition.y);
	} else {
		//m_ToPlace.push_back({
		//		columnPosition,
		//		blockPositionInColumn,
		//		type
		//	});

		// TODO: Send update to server instead
	}
}

ChunkColumn *Map::GetChunkColumnAt(int columnXPos, int columnZPos)
{
	if (m_ColumnMap.count(columnXPos) &&
	    m_ColumnMap[columnXPos].count(columnZPos))
		return m_ColumnMap[columnXPos][columnZPos].get();
	return nullptr;
}

glm::ivec2 Map::WorldToColumnPosition(const glm::ivec2 &worldPos)
{
	glm::ivec2 chunkPosition = glm::ivec2(worldPos.x / 16, worldPos.y / 16);
	if (worldPos.x < 0 && -worldPos.x % 16 != 0) {
		chunkPosition.x -= 1;
	}
	if (worldPos.y < 0 && -worldPos.y % 16 != 0) {
		chunkPosition.y -= 1;
	}
	return chunkPosition;
}

glm::ivec3 Map::WorldToChunkPosition(const glm::ivec3 &worldPos)
{
	glm::ivec3 m = worldPos;
	if (worldPos.x < 0) {
		m.x += 1;
	}
	if (worldPos.y < 0) {
		m.y += 1;
	}
	if (worldPos.z < 0) {
		m.z += 1;
	}
	glm::ivec3 chunkPosition = glm::ivec3(m.x / 16, m.y / 16, m.z / 16);
	if (worldPos.x < 0) {
		chunkPosition.x -= 1;
	}
	if (worldPos.y < 0) {
		chunkPosition.y -= 1;
	}
	if (worldPos.z < 0) {
		chunkPosition.z -= 1;
	}
	return chunkPosition;
}

glm::ivec3 Map::ColumnToWorldPosition(const glm::ivec2 &columnPosition,
				      const glm::ivec3 &blockPosition)
{
	return glm::ivec3(columnPosition.x * 16.0f + blockPosition.x,
			  blockPosition.y,
			  columnPosition.y * 16.0f + blockPosition.z);
}

glm::ivec3 Map::WorldToPositionInColumn(const glm::ivec3 &worldPosition)
{
	glm::ivec3 blockPositionInChunk = glm::ivec3(
		worldPosition.x % 16, worldPosition.y, worldPosition.z % 16);
	if (blockPositionInChunk.x < 0) {
		blockPositionInChunk.x = 16 + blockPositionInChunk.x;
	}
	if (blockPositionInChunk.z < 0) {
		blockPositionInChunk.z = 16 + blockPositionInChunk.z;
	}
	return blockPositionInChunk;
}

glm::ivec3 Map::WorldToPositionInChunk(const glm::ivec3 &worldPosition)
{
	glm::ivec3 blockPositionInChunk = glm::ivec3(worldPosition.x % 16,
						     worldPosition.y % 16,
						     worldPosition.z % 16);
	if (blockPositionInChunk.x < 0) {
		blockPositionInChunk.x = 16 + blockPositionInChunk.x;
	}
	if (blockPositionInChunk.y < 0) {
		blockPositionInChunk.y = 16 + blockPositionInChunk.y;
	}
	if (blockPositionInChunk.z < 0) {
		blockPositionInChunk.z = 16 + blockPositionInChunk.z;
	}
	return blockPositionInChunk;
}

uint32_t Map::GetMemoryUsage()
{
	uint32_t usage = 0;
	for (auto &columnX : m_ColumnMap) {
		for (auto &columnZ : columnX.second) {
			usage += columnZ.second->GetMemoryUsage();
		}
	}
	return usage;
}
