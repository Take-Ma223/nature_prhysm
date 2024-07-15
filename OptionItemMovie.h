#pragma once
#include "OptionItemBase.h"

namespace OptionItem {
	enum class Movie : int {
		OFF,
		ON_NORMAL,
		ON_CLEAR,
	};
}

class OptionItemMovie : public OptionItemBase
{
public:
	OptionItemMovie() :OptionItemBase() {}

protected:
	void initData() override {
		title = L"MOVIE";
		nameList = {
			L"OFF", 
			L"ON:NORMAL", 
			L"ON:CLEAR"
		};
		descriptionList = {
			L"“®‰æ‚ğÄ¶‚µ‚Ü‚¹‚ñ",
			L"“®‰æ‚ğÄ¶‚µ‚Ü‚·",
			L"“®‰æ‚ğÄ¶‚·‚é‚Æ‚«A•ˆ–ÊˆÈŠO‚ğ”ñ•\¦‚É‚µ‚Ä“®‰æ‚ğŒ©‚â‚·‚­‚µ‚Ü‚·"
		};
	}
};