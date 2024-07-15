#pragma once
#include "OptionItemBase.h"

class OptionItemSpeed : public OptionItemBase
{
public:
	int getVal() { return val[selectedIndex]; };

private:
public:
	OptionItemSpeed() :OptionItemBase() {}

private:
	std::vector<int> val;
	virtual void initData() override {
		title = L"SPEED";

		const int NOTE_OFFSET_NUM = 201;

		for (int i = 10; i <= 1600; i+=10) {
			val.push_back(i);

			auto Istring = std::to_wstring(i);
			nameList.push_back(Istring);
			descriptionList.push_back(L"‰¹•„‚Ì‘¬‚³‚ð" + Istring + L"‚É‡‚í‚¹‚Ü‚·");
			
		}
	}
};