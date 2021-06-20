//隠し曲の情報を格納する構造体
#include"DxLib.h"
#include"CONSTANT_VALUE.h"

#ifndef _STRUCT_SECRET
#define _STRUCT_SECRET

#define SECRET_SONG_NUMBER SONG_MAX_NUMBER

typedef struct SECRET {
	int total = 0;//隠し曲の数
	int *song_number;//隠し曲番号リスト 隠し曲の個数分だけその曲番号で埋められている配列
	BOOL *get_song_number;//隠し曲解禁リスト 各曲の解禁状況(0:非解禁 1:解禁)
	int all_get = 0;//全曲解禁しているか

	int Color;
	int ShadowColor;
	wchar_t Announce[256];
	int Announce_width;

	int secret_song_appear_number = -1;//隠し曲出現フラグ(-1:非出現 0~隠し曲リストの曲番号)
	int song_appear_number = -1;//隠し曲出現フラグ(-1:非出現 0~曲番号)
	wchar_t BGM[256] = L"\0";//専用選曲BGMのパス
	int beep_alarm = 0;//出現時にアラームを鳴らすかどうか
}SECRET;

#endif