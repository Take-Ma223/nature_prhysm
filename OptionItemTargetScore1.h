#pragma once
#include "OptionItemBase.h"

namespace OptionItem {
	enum class TargetScore1 : int {
		E,
		D,
		C,
		B,
		A,
		S,
		POINT_10000,
		FAST
	};
}

class OptionItemTargetScore1 : public OptionItemBase
{
public:
	OptionItemTargetScore1() :OptionItemBase() {}

private:
	virtual void initData() override {
		title = L"Target Score1";
		nameList = {
			L"E", 
			L"D", 
			L"C", 
			L"B", 
			L"A", 
			L"S", 
			L"10000", 
			L"MAX"
		};
		descriptionList = {
			L"ランクEに設定します",
			L"ランクDに設定します",
			L"ランクCに設定します",
			L"ランクBに設定します",
			L"ランクAに設定します",
			L"ランクSに設定します",
			L"10000点に設定します",
			L"MAXに設定します"
		};
	}
};