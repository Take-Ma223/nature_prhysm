#include"STRUCT_Song.h"
#include"STRUCT_OP.h"
#include<sys/stat.h>
#include<windows.h>

#ifndef _SONGCACHESAVELOAD
#define _SONGCACHESAVELOAD

//譜面を読み込まなければ得られない情報のキャッシュを作る

typedef struct SongCache {//譜面キャッシュ構造体
	FILETIME writeTime;//npsファイルの更新時間

	float bpm_suggested[5];//提案されるハイスピ設定に一番良いBPM
	unsigned short total_note;//総ノート数

	short LocalNotesAmount[2][9] = { { 0,0,0,0,0,0,0,0,0 },{ 0,0,0,0,0,0,0,0,0 } };//密度変化グラフ
	short ColorNotesAmount[2][9] = { { 0,0,0,0,0,0,0,0,0 },{ 0,0,0,0,0,0,0,0,0 } };//RGBCMYWBFそれぞれの数

	short global[2];//降水量(平均)
	short local[2];//局所降水量(0~1に正規化された色重みづけ配置タイミングの標準偏差 小さいほど局所難)
	short color[2];//色変化度
	short longNote[2];//LN率
	short unstability[2];//大気不安定度(ソフラン度)
	short chain[2];//縦連度

	short maxChords[2];//最大同時押し数

}SongCache;

extern void writeCacheToMusic(SongCache* Cache, Song* Music, int song_number, int difficulty);
extern void writeMusicToCache(SongCache* Cache, Song* Music, int song_number, int difficulty);
extern void SaveSongCache(SongCache Cache, Song* Music, int song_number, int difficulty);
extern int LoadSongCache(SongCache* Cache, Song* Music, int song_number, int difficulty);



#endif