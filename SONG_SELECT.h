#include"STRUCT_SECRET.h"
#include"STRUCT_ANDROID_CONTROLLER.h"
#include"STRUCT_LIST.h"
#include"CONSTANT_VALUE.h"
#include"STRUCT_CONFIG.h"
#include<vector>

#ifndef _SONG_SELECT
#define _SONG_SELECT

extern void SONG_SELECT(int *l_n,
	int *s_n,
	int *diff,
	OPTION *option, LIST *folder, Song *Music,
	int Button[3][4], int Button_Shutter, int *Key, char *Buf,
	int *debug, int *NumberOfSongs,
	int *result_count, int *result_rank_buf, SECRET *secret,
	ANDROID_CONTROLLER *AC, int *StageCount, SkillTestList *STList,
	CONFIG config);

extern wchar_t *announce_str(int StageCount, int PlayCount);

extern void DrawRadarLine(double angle);
extern int GetCylinderColor(double val, double range, unsigned int R, unsigned  int G, unsigned int B);
extern int GetRainbowColorForRadar(int val, int th);
extern void DrawHexagon(short, short, short, short, short, short);

extern void OptionValueChange(OPTION* Option, int option_select, int add);
extern void DrawOptionSentence(OPTION* Option, int option_select, CONFIG config, int FontHandle);


typedef struct SortSongListIndex {
	int index;
	int value;
}SortSongListIndex;


#endif