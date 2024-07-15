#pragma once
#include "OptionItemBase.h"

namespace OptionItem {
	enum class Windbreak : int {
		CLOSE_0,
		CLOSE_10,
		CLOSE_20,
		CLOSE_30,
		CLOSE_40,
		CLOSE_50,
		CLOSE_60,
		CLOSE_70,
		CLOSE_80,
		CLOSE_90,
		CLOSE_100
	};
}

class OptionItemWindbreak : public OptionItemBase
{
public:
	OptionItemWindbreak() :OptionItemBase() {}
	int getVal() { return windbreakVal[selectedIndex]; }
private:
	std::vector<int> windbreakVal = {
		0,
		100,
		200,
		300,
		400,
		500,
		600,
		700,
		800,
		900,
		1000
	};

	virtual void initData() override {
		title = L"WINDBREAK";
		nameList = {
			L"OFF",
			L"10%",
			L"20%",
			L"30%",
			L"40%",
			L"50%",
			L"60%",
			L"70%",
			L"80%",
			L"90%",
			L"100%"
		};
		descriptionList = {
			L"レーンカバーを下げずに演奏開始します",
			L"レーンカバーを10%下げて演奏開始します",
			L"レーンカバーを20%下げて演奏開始します",
			L"レーンカバーを30%下げて演奏開始します",
			L"レーンカバーを40%下げて演奏開始します",
			L"レーンカバーを50%下げて演奏開始します",
			L"レーンカバーを60%下げて演奏開始します",
			L"レーンカバーを70%下げて演奏開始します",
			L"レーンカバーを80%下げて演奏開始します",
			L"レーンカバーを90%下げて演奏開始します",
			L"レーンカバーを100%下げて演奏開始します"
		};
	}
};