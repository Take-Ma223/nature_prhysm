#include"DxLib.h"
#include"CONST_NOTE.h"

#ifndef _STRUCT_NOTE
#define _STRUCT_NOTE

const int TIMING_SHOW_RATIO = 1000;//表示用タイミングをミリ秒から何倍の精度で扱うか

typedef struct NOTE {
	short x = 0;//ノートのx座標
	short y = -128;//ノートのy座標
	int timing = 0;//表示用タイミング(マイクロ秒)
	int timing_real = 0;//叩く実際のタイミング(ミリ秒)
	int timing_init = 0;//LN始点の表示用タイミング保存用変数
	int timing_init_real = 0;//LNの実際に叩くタイミング保存用変数
	//char sound_number = 0;//鳴らす音の種類をビットで記入
	//int  sound_length = 0;//鳴らす音の長さ 
	char fall = 0;//0なら落ちてない,1なら落下中,2なら落ち切った
	char color_init = 0;//ノートの元の色
	char color = 0;//ノートの色(1~9の数字)(1:R 2:G 3:B 4:Y 5:C 6:M 7:W 8:K 9:F(FullColor) 10:ロングノート終点)0ならこのノートは使ってない
	float bpm = 1;//このノートのSCROLL抜きの速さ(BPM)
	float bpm_real = 1;//SCROLLの影響を考慮したこのノートの実際の速さ(BPM基準)
	BOOL hit = 0;//1:叩いた 0:叩いてない
	char group = 0;//ノートの種類(0:単ノート 1:ロングノート始点 2:ロングノート終点)
	BOOL LN_k = 0;//LNの終端で離すべきか(0:離さなくても良い 1:離さないといけない黒終点)
	short textLine = 0;//譜面データ上の行(見つけやすくする)
	BOOL snd = 0;//鳴らす音を長くするかどうか(0:通常 1で長くする) 長く鳴る音符は点滅する
	BOOL ignore = 0;//ワープ区間のため無視される音符なら1 それ以外0
}NOTE;


typedef struct BARLINE {//演奏画面で表示する小節線の構造体
	short x = 0;//ノートのx座標
	short y = -128;//ノートのy座標
	int timing = 0;//表示用タイミング(ミリ秒)
	int timing_real = 0;//叩く実際のタイミング(ミリ秒)
	//char sound_number = 0;//鳴らす音の種類をビットで記入
	//int  sound_length = 0;//鳴らす音の長さ 
	char fall = 0;//0なら落ちてない,1なら落下中,2なら落ち切った
	float bpm = 1;//このノート時点のBPM
	BOOL use = 0;//使用フラグ
}BARLINE;

#endif