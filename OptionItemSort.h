#pragma once
#include "OptionItemBase.h"

namespace OptionItem {
	enum class Sort : int {
		DEFAULT,
		LEVEL,
		SCORE,
		CLEAR_STATE,
		MIN_MISS,
		PLAY_COUNT,
		RADAR_TOTAL,
		GLOBAL,
		LOCAL,
		CHAIN,
		UNSTABILITY,
		STREAK,
		COLOR,
		RED_DENSITY,
		GREEN_DENSITY,
		BLUE_DENSITY,
		CYAN_DENSITY,
		MAGENTA_DENSITY,
		YELLOW_DENSITY,
		WHITE_DENSITY,
		BLACK_DENSITY,
		RAINBOW_DENSITY,
		MAX_BPM,
		MIN_BPM,
		MAX_CHORDS,
		VERSION,
	};
}

class OptionItemSort : public OptionItemBase
{
public:
	OptionItemSort() :OptionItemBase() {}

private:
	virtual void initData() override {
		title = L"SORT";
		nameList = {
			L"DEFAULT", 
			L"LEVEL", 
			L"SCORE", 
			L"CLEAR STATE",
			L"MIN MISS", 
			L"PLAY COUNT",
			L"RADAR TOTAL",
			L"GLOBAL",
			L"LOCAL",
			L"CHAIN",
			L"UNSTABILITY",
			L"STREAK",
			L"COLOR",
			L"RED",
			L"GREEN",
			L"BLUE",
			L"CYAN",
			L"MAGENTA",
			L"YELLOW",
			L"WHITE",
			L"BLACK",
			L"RAINBOW",
			L"MAX BPM",
			L"MIN BPM",
			L"MAX CHORDS",
			L"VERSION"
		};
		descriptionList = {
			 L"デフォルトの並び順にします",
			 L"難易度(降水確率)で並び替えます",
			 L"スコアで並び替えます",
			 L"クリア状態で並び替えます",
			 L"最小ミス数で並び替えます",
			 L"プレイ回数で並び替えます",
			 L"レーダーの合計値で並び替えます",
			 L"レーダーのGLOBAL値で並び替えます",
			 L"レーダーのLOCAL値で並び替えます",
			 L"レーダーのCHAIN値で並び替えます",
			 L"レーダーのUNSTABILITY値で並び替えます",
			 L"レーダーのSTREAK値で並び替えます",
			 L"レーダーのCOLOR値で並び替えます",
			 L"赤色音符の密度で並び替えます",
			 L"緑色音符の密度で並び替えます",
			 L"青色音符の密度で並び替えます",
			 L"水色音符の密度で並び替えます",
			 L"紫色音符の密度で並び替えます",
			 L"黄色音符の密度で並び替えます",
			 L"白色音符の密度で並び替えます",
			 L"黒色音符の密度で並び替えます",
			 L"虹色音符の密度で並び替えます",
			 L"最大BPMで並び替えます",
			 L"最小BPMで並び替えます",
			 L"最大同時押し数で並び替えます",
			 L"譜面が収録されたバージョンで並び替えます"
		};
	}
};