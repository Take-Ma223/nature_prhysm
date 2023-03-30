#include"STRUCT_SECRET.h"
#include"STRUCT_ANDROID_CONTROLLER.h"
#include"STRUCT_LIST.h"
#include"CONSTANT_VALUE.h"
#include"STRUCT_CONFIG.h"
#include<vector>
#include"STRUCT_IR_SETTING.h"

#ifndef _SONG_SELECT
#define _SONG_SELECT

extern void SONG_SELECT(int *l_n,
	int *s_n,
	int *diff,
	Option *option, LIST *folder, Song *Music,
	int Button[3][4], int Button_Shutter, int *Key, char *Buf,
	int *debug, int *NumberOfSongs,
	int *result_count, int *result_rank_buf, SECRET *secret,
	ANDROID_CONTROLLER *AC, int *StageCount, SkillTestList *STList,
	Config config, IR_SETTING* ir);

extern wchar_t *announce_str(int StageCount, int PlayCount);

extern void DrawRadarLine(double angle);
extern int GetCylinderColor(double val, double range, unsigned int R, unsigned  int G, unsigned int B);
extern int GetRainbowColorForRadar(int val, int th);
extern void DrawHexagon(short, short, short, short, short, short);

extern void OptionValueChange(Option* Option, int option_select, int add);
extern void DrawOptionSentence(Option* Option, OptionItem::Name option_select, Config config, int FontHandle);

extern int clearStateConverter(int clearState);


typedef struct SortSongListIndex {
	int index;
	int value;
}SortSongListIndex;


#endif