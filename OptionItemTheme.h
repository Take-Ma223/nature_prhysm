#pragma once
#include "OptionItemBase.h"
#include "FileUtil.h"

class OptionItemTheme : public OptionItemBase
{
public:
	OptionItemTheme() :OptionItemBase() {}

private:
	virtual void initData() override {
		title = L"THEME";
		nameList = FileUtil::getFolderList(wstring(L"img/themes"));

		for (int i = 0; i < nameList.size(); i++) {
			descriptionList.push_back(L"テーマスキンを変更します");
		}
	}
};