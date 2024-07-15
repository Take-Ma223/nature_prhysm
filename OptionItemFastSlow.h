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
			L"�^�C�~���O�̃Y����\�����܂���",
			L"�^�C�~���O�̃Y����\�����܂�(FAST:���F,SLOW:�ԐF)",
			L"�^�C�~���O�̃Y����\�����܂�(FAST:�ԐF,SLOW:���F)"
		};
	}
};