//インターネットランキング設定構造体
#include"DxLib.h"
#ifndef _STRUCT_IR_SETTING
#define _STRUCT_IR_SETTING

typedef struct IR_SETTING {//bpm_change
	wchar_t plaeyerName[17] = L"player";//プレイヤー名
	BOOL IR_Enable = TRUE;//IRを使うかどうか
	unsigned int rivalID = 0;//ライバルID
}IR_SETTING;


#endif