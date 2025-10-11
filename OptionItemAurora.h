#pragma once
#include "OptionItemBase.h"

namespace OptionItem {
	enum class Aurora : int {
		ON,
		OFF
	};
}

class OptionItemAurora : public OptionItemBase
{
public:
	OptionItemAurora() :OptionItemBase() {}

private:
	virtual void initData() override {
		title = L"AURORA";
		nameList = {
			L"ON",
			L"OFF"
		};
		descriptionList = {
			L"次に降る音符の色を表示します",
			L"次に降る音符の色を表示しません"
		};
	}
};