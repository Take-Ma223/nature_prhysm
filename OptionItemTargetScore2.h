#pragma once
#include "OptionItemBase.h"

namespace OptionItem {
	enum class TargetScore2 : int {
		RIVAL,
		MID,
		MEDIAN,
		NEXTRANK,
		TOP,
		LATEST
	};
}

class OptionItemTargetScore2 : public OptionItemBase
{
public:
	OptionItemTargetScore2() :OptionItemBase() {}

private:
	virtual void initData() override {
		title = L"Target Score2";
		nameList = {
			L"RIVAL", 
			L"AVERAGE", 
			L"MEDIAN", 
			L"NEXT RANK", 
			L"TOP", 
			L"LATEST"
		};
		descriptionList = {
			L"RIVAL�X�R�A�ɐݒ肵�܂�(�w�肷��RIVAL ID�̓^�C�g���Őݒ�)",
			L"�����L���O�̕��σX�R�A�ɐݒ肵�܂�",
			L"�����L���O�̒����l�X�R�A�ɐݒ肵�܂�",
			L"�����L���O�Ŏ�����1��̏��ʂ̃X�R�A�ɐݒ肵�܂�",
			L"�����L���O1�ʂ̃X�R�A�ɐݒ肵�܂�",
			L"�O��v���[�����Ƃ��̃X�R�A�ɐݒ肵�܂�"
		};
	}
};