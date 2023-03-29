#include "vbpch.h"

#include "Packets.h"

std::vector<uint8_t> AssembleChunkRequestPacket(int32_t x, int32_t z)
{
	std::vector<uint8_t> data;

	data.push_back(PacketType::ChunkRequest);

	std::array<unsigned char, 4> fourBuffer;
	fourBuffer = NumToBytes<int>(x); // X coord
	for (auto &b : fourBuffer)
		data.push_back(b);
	fourBuffer = NumToBytes<int>(z); // Z coord
	for (auto &b : fourBuffer)
		data.push_back(b);

	return data;
}

std::vector<uint8_t> AssemblePlayerInfoRequest(const std::string &username)
{
	std::vector<uint8_t> data;

	data.push_back(PacketType::PlayerInfoRequest);

	for (const char &c : username) {
		data.push_back(c);
	}
	data.push_back('\0');

	return data;
}

std::vector<uint8_t> AssemblePlayerInfoData(Player &player)
{
	std::vector<uint8_t> data;

	data.push_back(PacketType::PlayerInfoData);

	// position
	auto posBuffer = NumToBytes<glm::vec3>(player.GetPosition());
	for (auto &b : posBuffer)
		data.push_back(b);

	// rotation
	auto rotBuffer = NumToBytes<glm::vec2>(glm::vec2{
		player.GetCamera().GetPitch(), player.GetCamera().GetYaw() });
	for (auto &b : rotBuffer)
		data.push_back(b);

	// username
	for (const char &c : player.GetUsername()) {
		data.push_back(c);
	}
	data.push_back('\0');

	return data;
}

std::vector<uint8_t> AssemblePlayerActionRequest(PlayerActionRequest &request)
{
	std::vector<uint8_t> data;

	data.push_back(PacketType::ChunkUpdate);

	// block position
	auto posBuffer = NumToBytes<glm::ivec3>(request.pos);
	for (auto &b : posBuffer)
		data.push_back(b);

	// action type
	data.push_back((uint8_t)request.type);

	// block type
	std::array<unsigned char, 4> fourBuffer;
	fourBuffer = NumToBytes<uint32_t>(request.blockID);
	for (auto &b : fourBuffer)
		data.push_back(b);

	return data;
}