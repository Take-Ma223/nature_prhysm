#pragma once
#include "OptionItemBase.h"

namespace OptionItem {
	enum class TargetScore2 : int {
		RIVAL,
		MID,
		MEDIAN,
		NEXTRANK,
		TOP,
		LATEST
	};
}

class OptionItemTargetScore2 : public OptionItemBase
{
public:
	OptionItemTargetScore2() :OptionItemBase() {}

private:
	virtual void initData() override {
		title = L"Target Score2";
		nameList = {
			L"RIVAL", 
			L"AVERAGE", 
			L"MEDIAN", 
			L"NEXT RANK", 
			L"TOP", 
			L"LATEST"
		};
		descriptionList = {
			L"RIVALスコアに設定します(指定するRIVAL IDはタイトルで設定)",
			L"ランキングの平均スコアに設定します",
			L"ランキングの中央値スコアに設定します",
			L"ランキングで自分の1つ上の順位のスコアに設定します",
			L"ランキング1位のスコアに設定します",
			L"前回プレーしたときのスコアに設定します"
		};
	}
};