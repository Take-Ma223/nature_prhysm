#pragma once
#include "OptionItemBase.h"

namespace OptionItem {
	enum class MissEffect : int {
		BGM_FADE,
		OFF
	};
}

class OptionItemMissEffect : public OptionItemBase
{
public:
	OptionItemMissEffect() :OptionItemBase() {}

private:
	virtual void initData() override {
		title = L"MISS EFFECT";
		nameList = {
			L"BGM FADE",
			L"OFF"
		};
		descriptionList = {
			L"ミス時にBGMを消します",
			L"ミス時に何もしません"
		};
	}
};