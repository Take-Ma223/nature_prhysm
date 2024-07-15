#pragma once
#include "OptionItemBase.h"

namespace OptionItem {
	enum class ScoreGraph : int {
		OFF,
		ON
	};
}

class OptionItemScoreGraph : public OptionItemBase
{
public:
	OptionItemScoreGraph() :OptionItemBase() {}

private:
	virtual void initData() override {
		title = L"ScoreGraph";
		nameList = {
			L"OFF", 
			L"ON"
		};
		descriptionList = {
			L"スコアグラフを表示しません",
			L"スコアグラフを表示します(表示するグラフをTARGET SCOREで設定)"
		};
	}
};