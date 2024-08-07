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
			L"動画を再生しません",
			L"動画を再生します",
			L"動画を再生するとき、譜面以外を非表示にして動画を見やすくします"
		};
	}
};