#include"STRUCT_NOTE.h"
#include"STRUCT_CALCDIFF.h"
#include"STRUCT_SONG.h"
#include"STRUCT_SEQ.h"
#include"STRUCT_OP.h"
#include"STRUCT_EDIT_SCORE.h"
#include"STRUCT_CONFIG.h"

#ifndef _GAME_LOAD
#define _GAME_LOAD

extern void GAME_LOAD(int song_number, int difficulty, NOTE **note, BARLINE *barline, int *lane, int readflag, CALCDIFF *Cdiff, OPTION *Option, BPMC *bpmchange, SC *scrollchange, STOP_SE *stopSequence, int *hash, Song *Music, SongSub* MusicSub, int *TimetoEndScroll, int *playing_time, CONFIG config,
	double pitch = 1, 
	SCORE_CELL *score_cell_head = NULL,
	int SkillTestFlag = 0
);

extern void random_kaburanai(int *, int, int, int, int *, int *);
static void bpm_change_add(double bpm, BPMC *bpmchange, int *bpm_seq_counter, double time_counter, double note_offset_scroll, double *bc_timing_real, double *bc_timing, double scroll, double stopTimeSum);
//static void CalculateDifficultyRadar(NOTE**, CALCDIFF*);

const int CALC_MODE_NORMAL = 0;
const int CALC_MODE_RAINBOW = 1;

class DifficultyRadar {
public:
	DifficultyRadar(NOTE** note, int* nc, BPMC* bpmchange, STOP_SE* stopSequence, int time, int StartTime, int EndTime, int* TimingSame, short *NotesAmount, double BPM_suggest);//イニシャライザ,各色の個数も算出
	int NumberTranslation(int color);//rgbycmwdfの番号をrgbcmywdfに変換
	void GetLocalNotesGraph(short* LocalNotesGraph);//局所降水量の9分割グラフを求める

	int CalcGlobal(int Rainbow);//降水量(平均)
	int CalcLocal(int Rainbow);//最大局所降水量
	int index(NOTE**, int*, int*);//CalcLocal用の現在のncから最も最初にある音符のindexを返す関数

	int CalcColor(int StartTime, int EndTime, int Rainbow);//色変化度 (算出開始時間~算出終了時間)

	//int CalcRange(int Rainbow);//同時押し度
	int CalcLongNote(int Rainbow);//LN率
	int CalcUnstability();//大気安定度(ソフラン度)
	int CalcChain();//縦連度

	NOTE **note;
	int *nc;
	BPMC* bpmchange;
	STOP_SE *stopSequence;
	int time;
	int StartTime;
	int EndTime;
	int TotalNotes = 0;
	int TotalNotesRainbow = 0;
	int TotalNotesK = 0;
	int* TimingSame;

	int globalMax = 552;
	int localMax = 850;
	int colorMax = 350;
	int LNMax = 33;
	int unstabilityMax = 259;
	int chainMax = 120;
	double BPM_suggest = 0;

private:

};

#endif