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
			L"演奏画面の背景を暗くしません",
			L"演奏画面の背景を25%暗くします",
			L"演奏画面の背景を50%暗くします",
			L"演奏画面の背景を75%暗くします",
			L"演奏画面の背景を100%暗くします"
		};
	}
};