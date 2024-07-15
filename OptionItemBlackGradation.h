#pragma once
#include "OptionItemBase.h"

namespace OptionItem {
	enum class BlackGradation : int {
		ON,
		OFF
	};
}

class OptionItemBlackGradation : public OptionItemBase
{
public:
	OptionItemBlackGradation() :OptionItemBase() {}

private:
	virtual void initData() override {
		title = L"GRADATION";
		nameList = {
			L"ON",
			L"OFF"
		};
		descriptionList = {
			L"�����O�m�[�g���O���f�[�V�����\���ɂ��܂�",
			L"�����O�m�[�g���O���f�[�V�����\���ɂ��܂���"
		};
	}
};