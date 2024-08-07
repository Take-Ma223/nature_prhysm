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
			L"BGMの音量を0%にします",
			L"BGMの音量を10%にします",
			L"BGMの音量を20%にします",
			L"BGMの音量を30%にします",
			L"BGMの音量を40%にします",
			L"BGMの音量を50%にします",
			L"BGMの音量を60%にします",
			L"BGMの音量を70%にします",
			L"BGMの音量を80%にします",
			L"BGMの音量を90%にします",
			L"BGMの音量を100%にします"
		};
	}
};