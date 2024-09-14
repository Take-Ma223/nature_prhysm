#include"STRUCT_RESULT.h"
#include"STRUCT_OP.h"
#include"STRUCT_Song.h"
#include"STRUCT_NOTE.h"
#include"STRUCT_ANDROID_CONTROLLER.h"
#include"STRUCT_CONFIG.h"
#include"STRUCT_IR_SETTING.h"
#include"GradationNoteImage.h"
#include"NoteTextImage.h"

#ifndef _GAME
#define _GAME

extern void GAME(int song_number,
	int difficulty,
	RESULT* res,
	int* escape,
	Option* option,
	int* retryAble,
	int* debug,
	Song* Music,
	int Button[3][4],
	int Button_Shutter,
	int* Key,
	char* Buf,
	int secret,
	ANDROID_CONTROLLER* AC,
	Config config,
	IR_SETTING *ir,
	int SkillTestFlag = 0,
	double* GaugeVal = NULL,
	int* CourseCombo = 0,
	int* CourseMaxCombo = 0,
	int AllowExit = 1
	);

void drawNotes(SongSub& MusicSub, int difficulty, NOTE** note, int BlendMode, int BlendVal, int  H_NOTE[12], double flash, const int FLASH_VALUE, const int FLASH_VALUE_ALWAYS, NoteSearcher& noteSearcher, Option* option, GradationNoteImage& gradationLongNote, int  H_LNOTE[12], int  LN_flash[4], int  j_n_n[4], const int FLASH_VALUE_LN_PUSH, NoteSymbolImage& noteSymbol);

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