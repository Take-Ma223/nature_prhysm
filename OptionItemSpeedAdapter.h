#pragma once
#include "OptionItemBase.h"

namespace OptionItem {
	enum class SpeedAdapter : int {
		AVERAGE,
		MAX,
		GUST
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
			L"AVG",
			L"MAX",
			L"GUST",
		
		};
		descriptionList = {
			L"���ϑ��x(AVG)�ɃX�s�[�h�����킹�܂�",
			L"��������(MAX)�ɃX�s�[�h�����킹�܂�",
			L"��ԑ�������(GUST)�ɃX�s�[�h�����킹�܂�"
		};
	}
};