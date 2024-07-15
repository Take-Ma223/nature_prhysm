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
			L"BGM‚Ì‰¹—Ê‚ð0%‚É‚µ‚Ü‚·",
			L"BGM‚Ì‰¹—Ê‚ð10%‚É‚µ‚Ü‚·",
			L"BGM‚Ì‰¹—Ê‚ð20%‚É‚µ‚Ü‚·",
			L"BGM‚Ì‰¹—Ê‚ð30%‚É‚µ‚Ü‚·",
			L"BGM‚Ì‰¹—Ê‚ð40%‚É‚µ‚Ü‚·",
			L"BGM‚Ì‰¹—Ê‚ð50%‚É‚µ‚Ü‚·",
			L"BGM‚Ì‰¹—Ê‚ð60%‚É‚µ‚Ü‚·",
			L"BGM‚Ì‰¹—Ê‚ð70%‚É‚µ‚Ü‚·",
			L"BGM‚Ì‰¹—Ê‚ð80%‚É‚µ‚Ü‚·",
			L"BGM‚Ì‰¹—Ê‚ð90%‚É‚µ‚Ü‚·",
			L"BGM‚Ì‰¹—Ê‚ð100%‚É‚µ‚Ü‚·"
		};
	}
};