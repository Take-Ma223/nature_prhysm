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
			L"‰‰‘t‰æ–Ê‚Ì”wŒi‚ðˆÃ‚­‚µ‚Ü‚¹‚ñ",
			L"‰‰‘t‰æ–Ê‚Ì”wŒi‚ð25%ˆÃ‚­‚µ‚Ü‚·",
			L"‰‰‘t‰æ–Ê‚Ì”wŒi‚ð50%ˆÃ‚­‚µ‚Ü‚·",
			L"‰‰‘t‰æ–Ê‚Ì”wŒi‚ð75%ˆÃ‚­‚µ‚Ü‚·",
			L"‰‰‘t‰æ–Ê‚Ì”wŒi‚ð100%ˆÃ‚­‚µ‚Ü‚·"
		};
	}
};