#include"STRUCT_LIST.h"
#include"STRUCT_Song.h"

#ifndef _STRUCT_CALCDIFF
#define _STRUCT_CALCDIFF

typedef struct CALCDIFF {//難易度の自動算出要素(未実装)DIFFICULTY_RADAR
	int global = 0;//降水量(平均)
	int local = 0;//局所降水量(0~1に正規化された色重みづけ配置タイミングの標準偏差 小さいほど局所難)
	int color = 0;//色変化度
	int longNote = 0;//streak率
	int unstability = 0;//大気不安定度(ソフラン度)
	int chain = 0;//縦連度

	int level = 0;
	int level_local = 0;
	//int density = 0;//密度
	//int randa = 0;//乱打度
	//int tate = 0;//縦連度
	//int kaidan = 0;//階段度
	//int trill = 0;//トリル度
	//int color = 0;//複合度
	//int ln = 0;//ロングノート度
	//int rhythm = 0;//リズム難度
} CALCDIFF;

#endif