#pragma once
#include "OptionItemBase.h"

class OptionItemNoteOffset : public OptionItemBase
{
public:
	int getVal() { 
		if (val.size() == 0)return 0;
		return val[selectedIndex]; 
	};
	OptionItemNoteOffset() :OptionItemBase() {}

private:
	std::vector<int> val;
	virtual void initData() override {
		title = L"NOTE OFFSET";

		const int NOTE_OFFSET_NUM = 201;

		for (int i = -100; i <= 100; i++) {
			val.push_back(i);

			auto absIstring = std::to_wstring(abs(i));
			if (i < 0) {

				nameList.push_back(L"-" + absIstring + L"ms");
				descriptionList.push_back(L"�����̗����^�C�~���O��" + absIstring + L"�~���b�������܂�");
			}
			else if (i > 0) {
				nameList.push_back(L"+" + absIstring + L"ms");
				descriptionList.push_back(L"�����̗����^�C�~���O��" + absIstring + L"�~���b�x�����܂�");
			}
			else {
				nameList.push_back(absIstring + L"ms");
				descriptionList.push_back(L"�����̗����^�C�~���O�𒲐߂��܂���");
				
			}
		}
	}
};