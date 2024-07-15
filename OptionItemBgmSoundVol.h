#pragma once
#include "OptionItemBase.h"

namespace OptionItem {
	enum class BgmSoundVol : int {
		Vol_0,
		Vol_10,
		Vol_20,
		Vol_30,
		Vol_40,
		Vol_50,
		Vol_60,
		Vol_70,
		Vol_80,
		Vol_90,
		Vol_100
	};
}

class OptionItemBgmSoundVol : public OptionItemBase
{
public:
	OptionItemBgmSoundVol() :OptionItemBase() {}
	int getVolume() {return 255 * (double)getIndex() / (int)OptionItem::BgmSoundVol::Vol_100;}

private:
	virtual void initData() override {
		title = L"BGM VOL";
		nameList = {
			L"0%",
			L"10%",
			L"20%",
			L"30%",
			L"40%",
			L"50%",
			L"60%",
			L"70%",
			L"80%",
			L"90%",
			L"100%"
		};
		descriptionList = {
			L"BGM�̉��ʂ�0%�ɂ��܂�",
			L"BGM�̉��ʂ�10%�ɂ��܂�",
			L"BGM�̉��ʂ�20%�ɂ��܂�",
			L"BGM�̉��ʂ�30%�ɂ��܂�",
			L"BGM�̉��ʂ�40%�ɂ��܂�",
			L"BGM�̉��ʂ�50%�ɂ��܂�",
			L"BGM�̉��ʂ�60%�ɂ��܂�",
			L"BGM�̉��ʂ�70%�ɂ��܂�",
			L"BGM�̉��ʂ�80%�ɂ��܂�",
			L"BGM�̉��ʂ�90%�ɂ��܂�",
			L"BGM�̉��ʂ�100%�ɂ��܂�"
		};
	}
};