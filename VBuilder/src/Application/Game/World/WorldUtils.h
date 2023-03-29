#pragma once

struct BlockToPlace {
	glm::ivec2 ColumnPosition;
	glm::ivec3 PositionInColumn;
	int BlockType;
};

struct CompressedSet {
	int type = -1;
	int number = 0;
};

struct ChunkData {
	glm::vec3 position;
	std::shared_ptr<std::vector<CompressedSet> > data;
};