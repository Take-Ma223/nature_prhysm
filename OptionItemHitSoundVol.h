#pragma once
#include "OptionItemBase.h"

namespace OptionItem {
	enum class HitSoundVol : int {
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

class OptionItemHitSoundVol : public OptionItemBase
{
public:
	OptionItemHitSoundVol() :OptionItemBase() {}
	int getVolume() { return 255 * (double)getIndex() / (int)OptionItem::HitSoundVol::Vol_100; }

private:
	virtual void initData() override {
		title = L"HIT SOUND VOL";
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
			L"������@�������ɉ��ʂ�0%�ɂ��܂�",
			L"������@�������̉��ʂ�10%�ɂ��܂�",
			L"������@�������̉��ʂ�20%�ɂ��܂�",
			L"������@�������̉��ʂ�30%�ɂ��܂�",
			L"������@�������̉��ʂ�40%�ɂ��܂�",
			L"������@�������̉��ʂ�50%�ɂ��܂�",
			L"������@�������̉��ʂ�60%�ɂ��܂�",
			L"������@�������̉��ʂ�70%�ɂ��܂�",
			L"������@�������̉��ʂ�80%�ɂ��܂�",
			L"������@�������̉��ʂ�90%�ɂ��܂�",
			L"������@�������̉��ʂ�100%�ɂ��܂�"
		};
	};
};