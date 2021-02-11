//譜面のスクロールチェンジ、ストップの制御用構造体
#include"DxLib.h"
#ifndef _STRUCT_SEQ
#define _STRUCT_SEQ

typedef struct BPMC {//bpm_change
	BOOL use = 0;//使用フラグ
	double timing = 0;//GAME_time_passedのタイミング
	double timing_real = 0;//ゲームが始まってからの実際のタイミング
	double bpm = 1;
}BPMC;

typedef struct SC {//scroll_change
	BOOL use = 0;//使用フラグ
	double timing = 0;//GAME_time_passedのタイミング
	double timing_real = 0;//ゲームが始まってからの実際のタイミング
	double scroll = 1;
}SC;

typedef struct STOP_SE {//stop_sequence
	BOOL use = 0;//使用フラグ
	int timing = 0;//GAME_time_passedのタイミング
	double timing_real = 0;//ゲームが始まってからの実際のタイミング
	double stop_time = 1;//停止時間(単位はs)
}STOP_SE;

typedef struct WARP_SE {//warp_sequence
	BOOL use = 0;//使用フラグ
	int timing = 0;//GAME_time_passedのタイミング
	double timing_real = 0;//ゲームが始まってからの実際のタイミング
	double stop_time = 1;//停止時間(単位はs)
}WARP_SE;



#endif