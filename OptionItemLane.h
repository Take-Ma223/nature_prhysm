#pragma once
#include "OptionItemBase.h"

namespace OptionItem {
	enum class Lane : int {
		NONE,
		MIRROR,
		RANDOM,
		SUPER_RAND
	};
}

class OptionItemLane : public OptionItemBase
{
public:
	OptionItemLane() :OptionItemBase() {}

private:
	virtual void initData() override {
		title = L"LANE";
		nameList = {
			L"NONE",
			L"MIRROR",
			L"RANDOM",
			L"SUPER RAND"
		};
		descriptionList = {
			L"オプションを付けません",
			L"音符の出現するレーンが左右反転します",
			L"音符の出現するレーンがランダムに入れ替わります",
			L"音符の出現する場所がランダムに変わります"
		};
	}
};