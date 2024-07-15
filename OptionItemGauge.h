#pragma once
#include "OptionItemBase.h"

namespace OptionItem {
	enum class Gauge : int {
		NORMAL,
		HARD,
		SUPER_HARD,
		FC_ATTACK,
		PFC_ATTACK,
		NO_FAIL,
		SKILL_TEST
	};
}

class OptionItemGauge : public OptionItemBase
{
public:
	OptionItemGauge() :OptionItemBase() {}

private:
	virtual void initData() override {
		title = L"GAUGE";
		nameList = {
			L"NORMAL",
			L"HARD",
			L"SUPER HARD",
			L"FC ATTACK",
			L"PFC ATTACK",
			L"NO FAIL", 
			L"SKILL TEST"
		};
		descriptionList = { 
			L"�ʏ�̃Q�[�W�ł�"
			,L"���肪�����Q�[�W�ł�"
			,L"���肪���ɑ����񕜂����Ȃ��Q�[�W�ł�"
			,L"MISS���o���Ǝ��s�ɂȂ�Q�[�W�ł�"
			,L"MISS,GOOD���o���Ǝ��s�ɂȂ�Q�[�W�ł�"
			,L"�Ō�܂ŗV�ׂ錸��Ȃ��Q�[�W�ł�(�N���A�}�[�N�͕t���܂���)" 
		};
	}

	int itemCount() override { return descriptionList.size(); };

};

