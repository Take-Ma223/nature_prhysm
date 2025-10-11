#pragma once
#include "OptionItemBase.h"

namespace OptionItem {
	enum class Aurora : int {
		ON,
		OFF
	};
}

class OptionItemAurora : public OptionItemBase
{
public:
	OptionItemAurora() :OptionItemBase() {}

private:
	virtual void initData() override {
		title = L"AURORA";
		nameList = {
			L"ON",
			L"OFF"
		};
		descriptionList = {
			L"���ɍ~�鉹���̐F��\�����܂�",
			L"���ɍ~�鉹���̐F��\�����܂���"
		};
	}
};