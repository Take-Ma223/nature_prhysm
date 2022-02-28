#ifndef _SAVEDATA
#define _SAVEDATA

typedef struct SAVEDATA {//セーブデータ構造体
	unsigned int totalBootCount = 1;//総起動回数
	unsigned int totalPlayCount = 0;//総プレイ曲数
	unsigned int totalHighScore = 0;//総ハイスコア(正規)
	unsigned int totalHighScoreRainbow = 0;//総ハイスコア(虹)
	unsigned int totalHitNotes = 0;//叩いた音符の数
}SAVEDATA;

#endif