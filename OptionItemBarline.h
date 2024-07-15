#pragma once
#include "OptionItemBase.h"

namespace OptionItem {
	enum class Barline : int {
		ON,
		OFF
	};
}

class OptionItemBarline : public OptionItemBase
{
public:
	OptionItemBarline() :OptionItemBase() {}

private:
	virtual void initData() override {
		title = L"BARLINE";
		nameList = {
			L"ON",
			L"OFF"
		};
		descriptionList = {
			L"¬ßü‚ğ•\¦‚µ‚Ü‚·",
			L"¬ßü‚ğ•\¦‚µ‚Ü‚¹‚ñ"
		};
	}
};