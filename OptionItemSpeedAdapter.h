#pragma once
#include "OptionItemBase.h"

namespace OptionItem {
	enum class SpeedAdapter : int {
		AVERAGE,
		MAX,
		GUST
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
			L"AVG",
			L"MAX",
			L"GUST",
		
		};
		descriptionList = {
			L"平均速度(AVG)を基準にスピードを合わせます",
			L"早い音符(MAX)を基準にスピードを合わせます",
			L"一番早い音符(GUST)を基準にスピードを合わせます"
		};
	}
};