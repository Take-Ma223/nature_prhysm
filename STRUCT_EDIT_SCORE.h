#include"DxLib.h"
#include"CONST_NOTE.h"
#ifndef _STRUCT_EDIT_SCORE
#define _STRUCT_EDIT_SCORE

#define EDITSTEP 768//1小節のエディタでの分割数

typedef struct note_data {
	char color[4];//音符の種類を数値で格納
	BOOL bright[4];//点滅する音符かどうか(0:しない 1:する)
	char group[4];//ノートの種類(0:単ノート 1:ロングノート始点(使わない) 2:ロングノート終点) 譜面データを保存するときは終点かだけ分かればいい
	BOOL LN_k[4];//LNが黒終点か
}note_data;


#define COMMAND_KIND_MEASURE 0
#define COMMAND_KIND_BPM 1
#define COMMAND_KIND_SCROLL 2
#define COMMAND_KIND_SCROLL_BPM 3
#define COMMAND_KIND_STOP 4
#define COMMAND_KIND_STOPSTEP 5
#define COMMAND_KIND_HS 6
#define COMMAND_KIND_LETBPM 7

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
	ROW_DATA data = { 0 };
	SCORE_CELL *next=NULL;//次のセルへのポインタ
	SCORE_CELL *before=NULL;//前のセルへのポインタ
}SCORE_CELL;

#endif