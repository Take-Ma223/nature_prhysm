#include"STRUCT_NOTE.h"
#include"STRUCT_CALCDIFF.h"
#include"STRUCT_SONG.h"
#include"STRUCT_SEQ.h"
#include"STRUCT_OP.h"
#include"STRUCT_EDIT_SCORE.h"
#include"STRUCT_CONFIG.h"

#ifndef _GAME_LOAD
#define _GAME_LOAD

extern void GAME_LOAD(int song_number, int difficulty, NOTE **note, BARLINE *barline, int *laneCordinateX, int readflag, CALCDIFF *Cdiff, Option *Option, BPMC *bpmchange, SC *scrollchange, STOP_SE *stopSequence, Song *Music, SongSub* MusicSub, int *TimetoEndScroll, int *playing_time, Config config,
	double pitch = 1, 
	SCORE_CELL *score_cell_head = NULL,
	int SkillTestFlag = 0
);

extern void random_kaburanai(int *, int, int, int, int *, int *);
static void bpm_change_add(double bpm, BPMC *bpmchange, int *bpm_seq_counter, double time_counter, double note_offset_scroll, double *bc_timing_real, double *bc_timing, double scroll, double stopTimeSum);
//static void CalculateDifficultyRadar(NOTE**, CALCDIFF*);

const int CALC_MODE_NORMAL = 0;
const int CALC_MODE_RAINBOW = 1;

#endif