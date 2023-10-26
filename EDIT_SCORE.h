#include"STRUCT_EDIT_SCORE.h"
#include"STRUCT_Song.h"
#include"STRUCT_ANDROID_CONTROLLER.h"
#include"STRUCT_OP.h"
#include"STRUCT_CONFIG.h"

#ifndef _EDIT_SCORE
#define _EDIT_SCORE

#define CONTENTS_NOTE 1
#define CONTENTS_COMMAND 2
#define CONTENTS_END 3
#define CONTENTS_END_EDGE 4 //#ENDの後ろでリスト構造の最後

void EDIT_SCORE(SCORE_CELL *head, Song *Music, SongSub* MusicSub, Option *Option,
	int song_number, int difficulty, double init_scroll,
	int Button[3][4], int Button_Shutter, int* Key, char* Buf, double, double*,
	double* scale_score, int* step_array_number, int* measure_start_step, int* SwingBackBeatTime, ANDROID_CONTROLLER *AC, Config config, int SH_SONG);

int SAVE_EDIT_SCORE(SCORE_CELL *head, Song *Music, int song_number, int difficulty, double scroll);
wchar_t* generateRow(note_data& note);
wchar_t note_to_char(NoteColor color, NoteGroup group, BOOL bright, BOOL LN_k);
int gcd(int n, int m);
double calc_insert_passed_time(double init_bpm, double init_scroll, SCORE_CELL *p, SCORE_CELL *insert);//headからinsertまでの実時間をmsで計測し返す関数

typedef struct AreaSelect {//範囲選択構造体
	int start = 0;//選択エリアの始まりのstep_count
	int end = 0;//選択エリアの終わりのstep_count (カーソルの指す位置から1引くようにする)
	SCORE_CELL* StartCell = NULL;
	SCORE_CELL* EndCell = NULL;
}AreaSelect;

void DecideProcessArea(AreaSelect* area, SCORE_CELL* end);

void MirrorSelectArea(AreaSelect area);

void SolveLN(SCORE_CELL* end);//LNの不整合を良い感じにする

void DeleteSelectArea(SCORE_CELL** insert, AreaSelect area);

void CopySelectArea(int step_count, AreaSelect area, SCORE_CELL* CopyHead);

void PasteSelectArea(int step_count, SCORE_CELL** insert, int PasteRange, SCORE_CELL* CopyHead, double scale_m);


const int SWING_DIVISION = 12;//スウィング比計算の基準ステップ数





#endif