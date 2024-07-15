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
				descriptionList.push_back(L"音符の流れるタイミングを" + absIstring + L"ミリ秒早くします");
			}
			else if (i > 0) {
				nameList.push_back(L"+" + absIstring + L"ms");
				descriptionList.push_back(L"音符の流れるタイミングを" + absIstring + L"ミリ秒遅くします");
			}
			else {
				nameList.push_back(absIstring + L"ms");
				descriptionList.push_back(L"音符の流れるタイミングを調節しません");
				
			}
		}
	}
};