#pragma once
#include "OptionItemBase.h"
#include "FileUtil.h"

class OptionItemNoteSymbol : public OptionItemBase
{
public:
	OptionItemNoteSymbol() :OptionItemBase() {}

private:
	virtual void initData() override {
		title = L"NOTE SYMBOL";
		nameList = FileUtil::getFolderList(wstring(L"img/note_symbol"));

		for (int i = 0; i < nameList.size(); i++) {
			descriptionList.push_back(L"音符に重ねる画像を変更します");
		}
	}
};