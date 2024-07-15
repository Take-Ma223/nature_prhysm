#pragma once
#include "OptionItemBase.h"

namespace OptionItem {
	enum class Darkness : int {
		DARKNESS_0,
		DARKNESS_25,
		DARKNESS_50,
		DARKNESS_75,
		DARKNESS_100
	};
}

class OptionItemDarkness : public OptionItemBase
{
public:
	OptionItemDarkness() :OptionItemBase() {}

private:
	virtual void initData() override {
		title = L"NIGHT";
		nameList = {
			L"0%",
			L"25%",
			L"50%",
			L"75%",
			L"100%"
		};
		descriptionList = {
			L"���t��ʂ̔w�i���Â����܂���",
			L"���t��ʂ̔w�i��25%�Â����܂�",
			L"���t��ʂ̔w�i��50%�Â����܂�",
			L"���t��ʂ̔w�i��75%�Â����܂�",
			L"���t��ʂ̔w�i��100%�Â����܂�"
		};
	}
};