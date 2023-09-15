#include"DxLib.h"
#include "STRUCT_NOTE.h"
#ifndef _STRUCT_EDIT_SCORE
#define _STRUCT_EDIT_SCORE

#define EDITSTEP 3840//1小節のエディタでの分割数
#define EDITSTEP_SCALE_RATIO 5//768に対する倍率

typedef struct note_data {
	NoteColor color[4];//音符の種類
	BOOL isBright[4];//点滅する音符かどうか(0:しない 1:する)
	NoteGroup group[4];//ノートの種類(0:単ノート 1:ロングノート始点(使わない) 2:ロングノート終点) 譜面データを保存するときは終点かだけ分かればいい
	BOOL LN_k[4];//LNが黒終点か
}note_data;


#define COMMAND_KIND_MEASURE 8
#define COMMAND_KIND_BPM 1
#define COMMAND_KIND_SCROLL 2
#define COMMAND_KIND_SCROLL_BPM 3
#define COMMAND_KIND_STOP 4
#define COMMAND_KIND_STOPSTEP 5
#define COMMAND_KIND_HS 6
#define COMMAND_KIND_LETBPM 7
#define CONTENTS_KIND_DUMMY 10 //出力時に小節の先頭を指すセルとして使用

typedef struct command_data {
	char kind;//命令の種類
	double value[4];//命令の種類によって使われ方が決まる汎用変数
}command_data;


typedef union ROW_DATA {//データ行の音符、命令どっちかを格納する共用体
	note_data note;
	command_data command;
}ROW_DATA;


typedef struct SCORE_CELL {//譜面データを格納する双方向連結リスト構造の要素になる構造体
	int step = 0;//1小節をEDITSTEP分割した単位を使ったこの行の位置
	char content = 0;// 音符、命令どっちが入っているか(1, 音符 2:命令 3:#END)
	ROW_DATA data = { NoteColor::NONE };
	SCORE_CELL *next=NULL;//次のセルへのポインタ
	SCORE_CELL *before=NULL;//前のセルへのポインタ
}SCORE_CELL;

#endif