#pragma once
#include "OptionItemBase.h"

namespace OptionItem {
	enum class FxSoundVol : int {
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

class OptionItemFxSoundVol : public OptionItemBase
{
public:
	OptionItemFxSoundVol() :OptionItemBase() {}
	int getVolume() { return 255 * (double)getIndex() / (int)OptionItem::FxSoundVol::Vol_100; }

private:
	virtual void initData() override {
		title = L"FX VOL";
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
			L"���ʉ��̉��ʂ�0%�ɂ��܂�",
			L"���ʉ��̉��ʂ�10%�ɂ��܂�",
			L"���ʉ��̉��ʂ�20%�ɂ��܂�",
			L"���ʉ��̉��ʂ�30%�ɂ��܂�",
			L"���ʉ��̉��ʂ�40%�ɂ��܂�",
			L"���ʉ��̉��ʂ�50%�ɂ��܂�",
			L"���ʉ��̉��ʂ�60%�ɂ��܂�",
			L"���ʉ��̉��ʂ�70%�ɂ��܂�",
			L"���ʉ��̉��ʂ�80%�ɂ��܂�",
			L"���ʉ��̉��ʂ�90%�ɂ��܂�",
			L"���ʉ��̉��ʂ�100%�ɂ��܂�",
		};
	}
};