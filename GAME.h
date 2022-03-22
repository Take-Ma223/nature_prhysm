#include"STRUCT_RESULT.h"
#include"STRUCT_OP.h"
#include"STRUCT_Song.h"
#include"STRUCT_NOTE.h"
#include"STRUCT_ANDROID_CONTROLLER.h"
#include"STRUCT_CONFIG.h"
#include"STRUCT_IR_SETTING.h"

#ifndef _GAME
#define _GAME

extern void GAME(int song_number,
	int difficulty,
	RESULT* res,
	int* escape,
	OPTION* option,
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
	IR_SETTING *ir,
	int SkillTestFlag = 0,
	double* GaugeVal = NULL,
	int* CourseCombo = 0,
	int* CourseMaxCombo = 0,
	int AllowExit = 1
	);

extern void GAME_judge_note(int, NOTE**, int, int, int, int, int, int, int, int, int, int, int, int *, int *, int *);
extern void GAME_judge_dark_note(int, NOTE**, int, int, int, int, int*, int *);


extern void DrawFullComboRainbow(int *play, int *step, int Time, int baseTime, int effectResourceHandle, int *playingEffectHandle, int PFC);//フルコンボエフェクト


class SpeedBuffer {
	NOTE** note;
	int* noteIndex;//各レーン毎のインデックス

	float average = 0;

	static const int SIZE = 1;//先を見る個数

public:
	SpeedBuffer(NOTE** note, int* j_n_n);

	void updateAverage();

	float getAverage();
};



#endif