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
			L"���ߐ���\�����܂�",
			L"���ߐ���\�����܂���"
		};
	}
};