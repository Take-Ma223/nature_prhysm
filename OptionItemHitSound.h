#pragma once
#include "OptionItemBase.h"
#include "FileUtil.h"

class OptionItemHitSound : public OptionItemBase
{
public:
	OptionItemHitSound() :OptionItemBase() {}

private:
	virtual void initData() override {
		title = L"HIT SOUND";
		nameList = FileUtil::getFolderList(wstring(L"sound/hit_sound"));

		for (int i = 0; i < nameList.size(); i++) {
			descriptionList.push_back(L"������@�������̉���ύX���܂�");
		}
	}
};