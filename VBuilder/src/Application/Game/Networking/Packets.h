#pragma once

#include <stddef.h>
#include <string>
#include <array>

#include "../Player/Player.h"

enum PacketType {
	PlayerConnect,
	PlayerDisconnect,
	PlayerInfoRequest, // Get saved player data from file (if available)
	PlayerInfoData, // Data about a player to save, sent at a fixed interval from the client
	ChunkRequest, // Request from the client to send data about a chunk
	ChunkUpdate, // Request from the client to update a chunk
	ChunkContents // The contents of a chunk as requested by the client
	// TODO: Add server message to client // Send a message from the server to the client
};

enum ChunkUpdateType { PlaceBlockEvent, DestroyBlockEvent };

template <typename T> std::array<uint8_t, sizeof(T)> NumToBytes(T num)
{
	std::array<uint8_t, sizeof(T)> splitNum;
	memcpy(&splitNum[0], &num, sizeof(T));
	return splitNum;
}

template <typename T> T BytesToNum(uint8_t *num)
{
	T n;
	memcpy(&n, num, sizeof(T));
	return n;
}

std::vector<uint8_t> AssembleChunkRequestPacket(int32_t x, int32_t z);

std::vector<uint8_t> AssemblePlayerInfoRequest(const std::string &username);
std::vector<uint8_t> AssemblePlayerInfoData(Player &player);

struct PlayerActionRequest {
	ChunkUpdateType type;
	glm::ivec3 pos;
	uint32_t blockID;
};

std::vector<uint8_t> AssemblePlayerActionRequest(PlayerActionRequest &request);
