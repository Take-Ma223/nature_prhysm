#include "OptionItemBarline.h"
#include "OptionItemBgmSoundVol.h"
#include "OptionItemBlackGradation.h"
#include "OptionItemColor.h"
#include "OptionItemDarkness.h"
#include "OptionItemFastSlow.h"
#include "OptionItemFxSoundVol.h"
#include "OptionItemGauge.h"
#include "OptionItemHitSound.h"
#include "OptionItemHitSoundVol.h"
#include "OptionItemLane.h"
#include "OptionItemMovie.h"
#include "OptionItemNote.h"
#include "OptionItemNoteOffset.h"
#include "OptionItemScoreGraph.h"
#include "OptionItemSort.h"
#include "OptionItemSpeed.h"
#include "OptionItemTargetScore1.h"
#include "OptionItemTargetScore2.h"
#include "OptionItemTheme.h"
#include "OptionItemWindbreak.h"
#include "OptionItemSpeedAdapter.h"
#include "OptionItemNoteSymbol.h"

#ifndef _STRUCT_OP
#define _STRUCT_OP

namespace OptionItem {
	enum class Name : int {
		SPEED,
		SPEED_ADAPTER,
		COLOR,
		LANE,
		GAUGE,

		WINDBREAK,
		FAST_SLOW,
		BARLINE,
		DARKNESS,
		GRADATION,
		NOTE_OFFSET,
		SCORETARGET,
		TARGETSCORE1,
		TARGETSCORE2,
		THEME,
		NOTE,
		NOTE_SYMBOL,
		HITSOUND,
		HITSOUNDVOL,
		FXSOUNDVOL,
		BGMSOUNDVOL,
		MOVIE,
		SORT
	};





	enum class BannerColor : int {
		RED,
		GREEN,
		BLUE,
		CYAN,
		MAGENTA,
		YELLOW,
		WHITE,
		BLACK
	};
}

struct OP {//オプション構造体(ロード時に渡す値)
	std::vector < std::wstring > initParam = {
		L"120",
		L"FAST",
		L"NONE",
		L"NONE",
		L"NORMAL",

		L"OFF",
		L"ON:C/R",
		L"ON",
		L"25%",
		L"ON",
		L"0ms",
		L"OFF",
		L"E",
		L"RIVAL",
		L"",
		L"",
		L"",
		L"",
		L"100%",
		L"100%",
		L"100%",
		L"ON:NORMAL",
		L"DEFAULT",
	};

	OP() {
		for (int i = 0; i < list.size();i++) {
			list[i]->initData();
			list[i]->setIndexFromString(initParam[i]);
		}
	}

	OptionItemSpeed speed;
	OptionItemGauge gauge;
	OptionItemLane lane;
	OptionItemColor color;

	OptionItemSpeedAdapter speedAdapter;
	OptionItemWindbreak windbreak;
	OptionItemFastSlow fastSlow;
	OptionItemBarline barline;
	OptionItemDarkness darkness;
	OptionItemBlackGradation blackGradation;
	OptionItemNoteOffset noteOffset;
	OptionItemScoreGraph scoreGraph;
	OptionItemTargetScore1 targetScore1;
	OptionItemTargetScore2 targetScore2;
	OptionItemTheme theme;
	OptionItemNote note;
	OptionItemNoteSymbol noteSymbol;
	OptionItemHitSound hitSound;
	OptionItemHitSoundVol hitSoundVol;
	OptionItemFxSoundVol fxSoundVol;
	OptionItemBgmSoundVol bgmSoundVol;
	OptionItemMovie movie;
	OptionItemSort sort;

	vector<OptionItemBase*> list = {
		&speed,
		&speedAdapter,
		&lane,
		&color,
		&gauge,

		&windbreak,
		&fastSlow,
		&barline,
		&darkness,
		&blackGradation,
		&noteOffset,
		&scoreGraph,
		&targetScore1,
		&targetScore2,
		&theme,
		&note,
		&noteSymbol,
		&hitSound,
		&hitSoundVol,
		&fxSoundVol,
		&bgmSoundVol,
		&movie,
		&sort,
	};

	double speedVal = 1;//ハイスピ倍率
};

typedef struct Option {//オプション構造体(全体)
	OP op;

	static const int OPTION_NUM = 23;//オプションの数

	wchar_t* OptionName[OPTION_NUM] = { 
		L"SPEED",
		L"SPEED ADAPTER",
		L"LANE",
		L"COLOR",
		L"GAUGE",
		L"WINDBREAK",
		L"FAST/SLOW",
		L"BARLINE",
		L"NIGHT",
		L"GRADATION",
		L"NOTE OFFSET",
		L"SCORE GRAPH",
		L"TARGET SCORE1",
		L"TARGET SCORE2",
		L"THEME",
		L"NOTE",
		L"NOTE SYMBOL",
		L"HIT SOUND",
		L"HIT SOUND VOL",
		L"FX VOL",
		L"BGM VOL",
		L"MOVIE",
		L"SORT"
	};

	OptionItem::BannerColor bannerColor[OPTION_NUM] = {
		OptionItem::BannerColor::GREEN,
		OptionItem::BannerColor::GREEN,
		OptionItem::BannerColor::GREEN,
		OptionItem::BannerColor::GREEN,
		OptionItem::BannerColor::GREEN,

		OptionItem::BannerColor::BLUE,
		OptionItem::BannerColor::BLUE,
		OptionItem::BannerColor::BLUE,
		OptionItem::BannerColor::BLUE,
		OptionItem::BannerColor::BLUE,
		OptionItem::BannerColor::BLUE,

		OptionItem::BannerColor::RED,
		OptionItem::BannerColor::RED,
		OptionItem::BannerColor::RED,

		OptionItem::BannerColor::YELLOW,
		OptionItem::BannerColor::YELLOW,
		OptionItem::BannerColor::YELLOW,
		OptionItem::BannerColor::YELLOW,
		OptionItem::BannerColor::YELLOW,
		OptionItem::BannerColor::YELLOW,
		OptionItem::BannerColor::YELLOW,
		OptionItem::BannerColor::YELLOW,

		OptionItem::BannerColor::CYAN,
	};

	int H_SENT;//画像ハンドル

 }Option;

#endif