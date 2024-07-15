#pragma once
#include "OptionItemBase.h"

namespace OptionItem {
	enum class Color : int {
		NONE,
		RGB_CHANGE,
		RGB_RAND,
		RGB_RAND_PLUS,
		SUPER_RAND,
		SUPER_RAND_PLUS,
		RGB_ONLY,
		CMY_ONLY,
		W_ONLY,
		RAINBOW
	};
}

class OptionItemColor : public OptionItemBase
{
public:
	OptionItemColor() :OptionItemBase() {}
	bool isRainbow() { return getIndex() == static_cast<int>(OptionItem::Color::RAINBOW); }

private:
	virtual void initData() override {
		title = L"COLOR";
		nameList = {
			L"NONE",
			L"RGB CHANGE",
			L"RGB RAND",
			L"RGB RAND+",
			L"SUPER RAND",
			L"SUPER RAND+",
			L"RGB ONLY",
			L"CMY ONLY",
			L"W ONLY",
			L"RAINBOW"
		};
		descriptionList = {
			L"オプションを付けません",
			L"音符全体のRGB値がランダムに入れ替わります",
			L"音符それぞれのRGB値がランダムに入れ替わります",
			L"音符それぞれのRGB値がランダムに入れ替わります(ロングノート終点も変化)",
			L"音符の色がランダムに変わります",
			L"音符の色がランダムに変わります(ロングノート終点も変化)",
			L"音符の色が赤,緑,青のみになります(スコアは保存されません)",
			L"音符の色が水色,紫,黄色のみになります(スコアは保存されません)",
			L"音符の色が白のみになります(スコアは保存されません)",
			L"音符の色が虹色のみになります(スコアは別に保存されます)"
		};
	}
};