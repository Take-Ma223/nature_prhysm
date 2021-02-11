#include"STRUCT_RESULT.h"
#include"STRUCT_OP.h"
#include"STRUCT_Song.h"
#include"STRUCT_NOTE.h"
#include"STRUCT_ANDROID_CONTROLLER.h"
#include"STRUCT_CONFIG.h"

#ifndef _GAME
#define _GAME

extern void GAME(int song_number,
	int difficulty,
	RESULT* res,
	int* escape,
	OPTION* Ooption,
	int* retryAble,
	int* debug,
	Song* Music,
	int Button[3][4],
	int Button_Shutter,
	int* Key,
	char* Buf,
	int secret,
	ANDROID_CONTROLLER* AC,
	CONFIG config,
	int SkillTestFlag = 0,
	double* GaugeVal = NULL,
	int* CourseCombo = 0,
	int* CourseMaxCombo = 0,
	int AllowExit = 1
	);

extern void GAME_judge_note(int, NOTE**, int, int, int, int, int, int, int, int, int, int, int, int *, int *, int *);
extern void GAME_judge_dark_note(int, NOTE**, int, int, int, int, int*, int *);


extern void DrawFullComboRainbow(int *play, int *step, int Time, int baseTime, int effectResourceHandle, int *playingEffectHandle, int PFC);//フルコンボエフェクト


extern void DebugInitializeProcess(
	int* combo,
	int* CourseCombo,
	int* CourseMaxCombo,
	double* score,
	int* SKY_PERFECT,
	int* PERFECT,
	int* GOOD,
	int* MISS,
	int* TimePerfect,
	int* TimeGood,
	int* TimeMiss,
	int* TimeNextHour,
	int* MAX_COMBO,
	double* gauge,
	int* bgmplay,
	int* bcc,
	int* scc,
	double* cscroll,
	double scrollchange_scroll,
	double* sc_timing,
	double* real_timing,

	int* stop_se_c,
	double* stop_time_sum,
	double* stop_time,
	int* stopFlag,


	int* j_n_n,
	int* j_dn_n,
	int* j_dn_push_n,
	int* LN_flag,
	int* LN_flash,
	BARLINE* barline,
	NOTE** note
);//デバッグ時の移動処理などの共通処理

#endif