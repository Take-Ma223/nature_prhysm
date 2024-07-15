#pragma once
#include "OptionItemBase.h"

namespace OptionItem {
	enum class FastSlow : int {
		OFF,
		ON_FAST_CYAN,
		ON_FAST_RED
	};
}

class OptionItemFastSlow : public OptionItemBase
{
public:
	OptionItemFastSlow() :OptionItemBase() {}

private:
	virtual void initData() override {
		title = L"FAST/SLOW";
		nameList = {
			L"OFF",
			L"ON:C/R",
			L"ON:R/C"
		};
		descriptionList = {
			L"タイミングのズレを表示しません",
			L"タイミングのズレを表示します(FAST:水色,SLOW:赤色)",
			L"タイミングのズレを表示します(FAST:赤色,SLOW:水色)"
		};
	}
};