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
			L"�����NE�ɐݒ肵�܂�",
			L"�����ND�ɐݒ肵�܂�",
			L"�����NC�ɐݒ肵�܂�",
			L"�����NB�ɐݒ肵�܂�",
			L"�����NA�ɐݒ肵�܂�",
			L"�����NS�ɐݒ肵�܂�",
			L"10000�_�ɐݒ肵�܂�",
			L"MAX�ɐݒ肵�܂�"
		};
	}
};