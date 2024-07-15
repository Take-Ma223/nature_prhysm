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


#ifndef _STRUCT_OP
#define _STRUCT_OP

namespace OptionItem {
	enum class Name : int {
		SPEED,
		GAUGE,
		LANE,
		COLOR,
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
		L"NORMAL",
		L"NONE",
		L"NONE",
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
		L"100%",
		L"100%",
		L"100%",
		L"ON:NORMAL",
		L"NAME",
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
	OptionItemHitSound hitSound;
	OptionItemHitSoundVol hitSoundVol;
	OptionItemFxSoundVol fxSoundVol;
	OptionItemBgmSoundVol bgmSoundVol;
	OptionItemMovie movie;
	OptionItemSort sort;

	vector<OptionItemBase*> list = {
		&speed,
		&gauge,
		&lane,
		&color,
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

	static const int OPTION_NUM = 21;//オプションの数

	wchar_t* OptionName[OPTION_NUM] = { 
		L"SPEED",
		L"GAUGE",
		L"LANE",
		L"COLOR",
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

		OptionItem::BannerColor::CYAN,
	};

	int H_SENT;//画像ハンドル

	//選曲画面で使うオプション名称配列
	//const int* ArrayOptionNum[OPTION_NUM] = {
	//	&SPEED_NUM,
	//	&GAUGE_NUM,
	//	&LANE_NUM,
	//	&COLOR_NUM,
	//	&WINDBREAK_NUM,
	//	&FAST_SLOW_NUM,
	//	&BARLINE_NUM,
	//	&DARKNESS_NUM,
	//	&BLACK_GRADATION_NUM,
	//	&NOTE_OFFSET_NUM,
	//	&SCORE_GRAPH_NUM,
	//	&TARGET_SCORE1_NUM,
	//	&TARGET_SCORE2_NUM,
	//	&THEME_NUM,
	//	&NOTE_NUM,
	//	&HITSOUND_NUM,
	//	&HIT_SOUND_VOL_NUM,
	//	&FX_SOUND_VOL_NUM,
	//	&BGM_SOUND_VOL_NUM,
	//	&MOVIE_NUM,
	//	&SORT_NUM };//各オプションの数

	//wchar_t** ArrayOptionSent[OPTION_NUM] = {
	//	sent_speed,
	//	sent_gauge,
	//	sent_lane,
	//	sent_color,
	//	sent_windbreak,
	//	sent_fastSlow,
	//	sent_barline,
	//	sent_darkness,
	//	sent_blackGradation,
	//	sent_noteOffset,
	//	sent_scoreTarget,
	//	sent_targetScore1,
	//	sent_targetScore2,
	//	&sent_theme,
	//	&sent_note,
	//	&sent_hitSound,
	//	sent_hit_sound_vol,
	//	sent_fx_sound_vol,
	//	sent_bgm_sound_vol,
	//	sent_movie,
	//	sent_sort };//説明文

	//wchar_t** ArrayOptionKindName[OPTION_NUM] = { 
	//	speed,
	//	gauge,
	//	lane,
	//	color,
	//	windbreak,
	//	fastSlow,
	//	barline,
	//	darkness,
	//	blackGradation,
	//	noteOffset,
	//	scoreTarget,
	//	targetScore1,
	//	targetScore2,
	//	theme,
	//	note,
	//	hitSound,
	//	hitSoundVol,
	//	fxSoundVol,
	//	bgmSoundVol,
	//	movie,
	//	sort };//オプション名称

	//int* ArrayValue[OPTION_NUM] = {
	//	&(op.speed),
	//	(int*)&(op.gauge),
	//	(int*)&(op.lane),
	//	(int*)&(op.color),
	//	(int*)&(op.windbreak),
	//	(int*)&(op.fastSlow),
	//	(int*)&(op.barline),
	//	(int*)&(op.darkness),
	//	(int*)&(op.blackGradation),
	//	&(op.noteOffset),
	//	(int*)&(op.scoreGraph),
	//	(int*)&(op.targetScore1),
	//	(int*)&(op.targetScore2),
	//	&(op.theme),
	//	&(op.note),
	//	&(op.hitSound),
	//	(int*)&(op.hitSoundVol),
	//	(int*)&(op.fxSoundVol),
	//	(int*)&(op.bgmSoundVol),
	//	(int*)&(op.movie),
	//	(int*)&(op.sort) };//選んでいるオプションの値

	
 }Option;

#endif