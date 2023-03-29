#include "vbpch.h"

#include "AssetInfo.h"

struct ItemData {
	bool Locked = false;
	std::vector<ItemInfo> Data;
};

static ItemData s_Data;

bool ItemInfoManager::AddItem(const ItemInfo &info)
{
	if (!s_Data.Locked) {
		s_Data.Data.push_back(info);
		return true;
	}
	VB_WARN("Item data is locked!");
	return false;
}

void ItemInfoManager::ClearAllItems()
{
	if (!s_Data.Locked)
		s_Data.Data.clear();
	else
		VB_WARN("Item data is locked!");
}

void ItemInfoManager::SetItemsLocked(bool locked)
{
	s_Data.Locked = locked;
}

int ItemInfoManager::GetIDByName(const std::string &name)
{
	for (int i = 0; i < s_Data.Data.size(); i++) {
		if (s_Data.Data[i].Name == name)
			return i;
	}
	return -1;
}

const ItemInfo *ItemInfoManager::GetItemByID(const int &id)
{
	if (id >= 0 && id < s_Data.Data.size())
		return &s_Data.Data[id];
	return nullptr;
}
