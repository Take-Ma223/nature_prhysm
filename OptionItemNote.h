#pragma once
#include "OptionItemBase.h"
#include "FileUtil.h"

class OptionItemNote : public OptionItemBase
{
public:
	OptionItemNote() :OptionItemBase() {}

private:
	virtual void initData() override {
		title = L"NOTE";
		nameList = FileUtil::getFolderList(wstring(L"img/notes"));

		for (int i = 0; i < nameList.size(); i++) {
			descriptionList.push_back(L"‰¹•„‚ð•ÏX‚µ‚Ü‚·");
		}
	}
};