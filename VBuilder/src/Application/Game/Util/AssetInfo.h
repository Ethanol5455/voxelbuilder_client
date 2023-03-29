#pragma once

#include "Renderer/Texture.h"

enum class ItemType { Air, BlockCube, BlockCross, UserItem };

struct ItemInfo {
	ItemType Type = ItemType::UserItem;
	bool IsTransparent = false;
	bool ShowInInventory = true;
	std::string Name = "UNKNOWN";
	TextureCoordinates TopTexCoords =
		Texture::ExtractCoordinates(glm::vec2(10, 10), { 0, 9 });
	TextureCoordinates SideTexCoords =
		Texture::ExtractCoordinates(glm::vec2(10, 10), { 0, 9 });
	TextureCoordinates BottomTexCoords =
		Texture::ExtractCoordinates(glm::vec2(10, 10), { 0, 9 });
};

class ItemInfoManager {
    public:
	static bool AddItem(const ItemInfo &info);
	static void ClearAllItems();

	static void SetItemsLocked(bool locked);

	// Warning: Uses sequential search operation!
	static int GetIDByName(const std::string &name);
	static const ItemInfo *GetItemByID(const int &id);
};
