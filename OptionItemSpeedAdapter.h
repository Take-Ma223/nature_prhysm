#pragma once
#include "OptionItemBase.h"

namespace OptionItem {
	enum class SpeedAdapter : int {
		MID,
		FAST,
		MAX
	};
}

class OptionItemSpeedAdapter : public OptionItemBase
{
public:
	OptionItemSpeedAdapter() :OptionItemBase() {}

private:
	virtual void initData() override {
		title = L"Speed Adapter";
		nameList = {
			L"MID",
			L"FAST",
			L"MAX",
		
		};
		descriptionList = {
			L"中央速度(MID)にスピードを合わせます",
			L"早い音符(FAST)にスピードを合わせます",
			L"一番早い音符(MAX)にスピードを合わせます"
		};
	}
};