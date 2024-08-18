#pragma once
#include "OptionItemBase.h"

namespace OptionItem {
	enum class SpeedAdapter : int {
		MID,
		FAST,
		MAX
	};
}

class OptionItemSpeedAdapter : public OptionItemBase
{
public:
	OptionItemSpeedAdapter() :OptionItemBase() {}

private:
	virtual void initData() override {
		title = L"Speed Adapter";
		nameList = {
			L"MID",
			L"FAST",
			L"MAX",
		
		};
		descriptionList = {
			L"�������x(MID)�ɃX�s�[�h�����킹�܂�",
			L"��������(FAST)�ɃX�s�[�h�����킹�܂�",
			L"��ԑ�������(MAX)�ɃX�s�[�h�����킹�܂�"
		};
	}
};