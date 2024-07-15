#pragma once
#include "OptionItemBase.h"

namespace OptionItem {
	enum class Gauge : int {
		NORMAL,
		HARD,
		SUPER_HARD,
		FC_ATTACK,
		PFC_ATTACK,
		NO_FAIL,
		SKILL_TEST
	};
}

class OptionItemGauge : public OptionItemBase
{
public:
	OptionItemGauge() :OptionItemBase() {}

private:
	virtual void initData() override {
		title = L"GAUGE";
		nameList = {
			L"NORMAL",
			L"HARD",
			L"SUPER HARD",
			L"FC ATTACK",
			L"PFC ATTACK",
			L"NO FAIL", 
			L"SKILL TEST"
		};
		descriptionList = { 
			L"通常のゲージです"
			,L"減りが早いゲージです"
			,L"減りが非常に早く回復も少ないゲージです"
			,L"MISSを出すと失敗になるゲージです"
			,L"MISS,GOODを出すと失敗になるゲージです"
			,L"最後まで遊べる減らないゲージです(クリアマークは付きません)" 
		};
	}

	int itemCount() override { return descriptionList.size(); };

};

