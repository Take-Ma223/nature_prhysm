#pragma once
#include "OptionItemBase.h"
#include <map>

namespace OptionItem {
	enum class LaneWidth : int {
		LaneWidth_EXTRA_WIDE,
		LaneWidth_WIDE,
		LaneWidth_NORMAL,
		LaneWidth_NARROW,
		LaneWidth_EXTRA_NARROW,
	};
}

class OptionItemLaneWidth : public OptionItemBase
{
public:
	OptionItemLaneWidth() :OptionItemBase() {}

	static double getLaneWidth(OptionItem::LaneWidth laneWidth) {
		std::map<OptionItem::LaneWidth, double> width_ratio;
		width_ratio[OptionItem::LaneWidth::LaneWidth_EXTRA_WIDE] = 1.0;
		width_ratio[OptionItem::LaneWidth::LaneWidth_WIDE] = 0.875;
		width_ratio[OptionItem::LaneWidth::LaneWidth_NORMAL] = 0.75;
		width_ratio[OptionItem::LaneWidth::LaneWidth_NARROW] = 0.625;
		width_ratio[OptionItem::LaneWidth::LaneWidth_EXTRA_NARROW] = 0.5;

		return width_ratio[laneWidth];
	}

private:
	virtual void initData() override {
		title = L"LANE WIDTH";
		nameList = {
			L"EXTRA WIDE",
			L"WIDE",
			L"NORMAL",
			L"NARROW",
			L"EXTRA NARROW",
		};
		descriptionList = {
			L"���[��������ɍL�����܂�(100%)",
			L"���[�������L�����܂�(87.5%)",
			L"���[�����𕁒ʂɂ��܂�(75%)",
			L"���[�������������܂�(62.5%)",
			L"���[��������ɋ������܂�(50%)",
		};
	}
};