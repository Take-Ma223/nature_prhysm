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
			L"�X�R�A�O���t��\�����܂���",
			L"�X�R�A�O���t��\�����܂�(�\������O���t��TARGET SCORE�Őݒ�)"
		};
	}
};