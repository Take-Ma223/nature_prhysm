#include"STRUCT_RESULT.h"
#include"STRUCT_OP.h"
#include"STRUCT_SONG.h"
#include"STRUCT_ANDROID_CONTROLLER.h"
#include"STRUCT_LIST.h"
#include"STRUCT_CONFIG.h"
#include"STRUCT_IR_SETTING.h"

#ifndef _SHOW_RESULT
#define _SHOW_RESULT

extern void SHOW_RESULT(RESULT res, OPTION *option, int song_number, int difficulty, int *debug, Song *Music, int Button[3][4], int Button_Shutter, int *Key, char *Buf, ANDROID_CONTROLLER *AC, int TryCount,
	SkillTestList *STList,
	int list_number,
	CONFIG config,
	IR_SETTING* ir,
	int SkillTestFlag = 0
);

extern void ShowJudgeNumber(int Number, int y, int Flag, double draw_alpha, int *H_SCORE_NUMBER, int digit[5], int SlideX, int SlideY);
extern void ReplaceStr(wchar_t *Str);
extern void ReplaceTweetStr(wchar_t *Str);
extern void GetBG(wchar_t* SearchPass, wchar_t* ImagePass);

#endif