#include"DxLib.h"
#include<string>
#include<vector>
#ifndef _STRUCT_SONG
#define _STRUCT_SONG

using namespace std;

enum Radar
{
	Global, Local, Chain, Unstability, LongNote, Color
};

enum RainbowMode {
	General=0,
	Rainbow=1
};

enum UnlockState {
	Normal = 0,
	Secret = 1,
	Unlocked = 2
};

class Song {
public:
	BOOL exist[5] = { 0,0,0,0,0 };//この難易度があるかどうか(0:無い 1:ある)
	UnlockState secret = UnlockState::Normal;//隠し曲かどうか(0:通常曲 1:隠し曲 2:解禁済み)
	unsigned int StrColor[5] = { 0,0,0,0,0 };//曲名を表示する色(GetColorの値)
	unsigned int StrShadowColor[5] = { 0,0,0,0,0 };//曲名の枠を表示する色(GetColorの値)
	wchar_t* KindFolder;//種類フォルダの名前(songs/folder のfolder部分)

	wchar_t* FolderPath;//曲フォルダのパス(songs/folder/song)
	wchar_t* SongPath[5];//npsファイルへのパス[]:難易度(とりあえず5個、Sunny:1 Cloudy:2 Rainy:3)
	wchar_t* SaveFolder;//セーブファイルのあるフォルダへのパス
	wchar_t* CacheFolder;//キャッシュファイルのあるフォルダへのパス
	wchar_t* RivalSaveFolder;//ライバルセーブファイルのあるフォルダへのパス
						 //char Comment[512];//コメント
	//wchar_t title[5][128]  = { L"\0", L"\0", L"\0", L"\0", L"\0" };//タイトル
	wchar_t* title[5] = { NULL,NULL,NULL,NULL,NULL };//タイトル
	wchar_t* genre[5] = { NULL,NULL,NULL,NULL,NULL };//ジャンル
	wchar_t* artist[5] = { NULL,NULL,NULL,NULL,NULL };//アーティスト
	//wchar_t* detail[5] = { NULL,NULL,NULL,NULL,NULL };

	vector<vector<wstring>> detail;//詳細

	wchar_t* wavpath[5] = { NULL,NULL,NULL,NULL,NULL };//音源のパス(難易度によって音源変えたり？できるように10個分)
	wchar_t* jacketpath[5] = { NULL,NULL,NULL,NULL,NULL };//ジャケット画像のパス(難易度によって画像変えたり？できるように10個分)
	wchar_t* jacketpath2[5] = { NULL,NULL,NULL,NULL,NULL };//ジャケット2画像のパス(難易度によって画像変えたり？できるように10個分)
	wchar_t* jinglepath[5] = { NULL,NULL,NULL,NULL,NULL };//ジングル音源
	wchar_t* moviePath[5] = { NULL,NULL,NULL,NULL,NULL };//ムービーパス

	double bpm[5];//最初のBPM
	short bpmmin[5];//選曲画面で表示するBPMの最大値
	short bpmmax[5];//選曲画面で表示するBPMの最大値
	float bpm_suggested[5];//提案されるハイスピ設定に一番良いBPM
	unsigned char level[5];//レベル(降水確率)
	int songoffset[5];
	int noteoffset[5];
	int movieoffset[5];
	int demostart[5];//テキストでは秒で指定するが内部ではミリ秒で指定
	int TimeToEnd[5];//演奏終了タイミング

	unsigned short total_note[5];//総ノート数

	char season[5] = { 0,0,0,0,0 };//この曲の季節(0:無季節,1:春,2:夏,3:秋,4:冬)

	short LocalNotesAmount[2][5][9] = {
		{{0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0} },
		{{0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0} }
	};//密度変化グラフ

	short ColorNotesAmount[2][5][9] = { 
		{{0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0} },
		{{0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0} }
	};//RGBCMYWBFそれぞれの密度

	short global[2][5];//降水量(平均)
	short local[2][5];//局所降水量(0~1に正規化された色重みづけ配置タイミングの標準偏差 小さいほど局所難)
	short color[2][5];//色変化度
	short longNote[2][5];//LN率
	short unstability[2][5];//大気不安定度(ソフラン度)
	short chain[2][5];//縦連度

	unsigned short version[5];//追加されたバージョン(小数を100倍して保存)

	short maxChords[2][5];//最大同時押し数

	BOOL isTop(Radar radar, int difficiulty, int rainbow = 0) {
		short radarValue[6] = {
			global[rainbow][difficiulty],
			local[rainbow][difficiulty],
			chain[rainbow][difficiulty],
			unstability[rainbow][difficiulty],
			longNote[rainbow][difficiulty],
			color[rainbow][difficiulty],
		};

		if (radarValue[radar] >= radarValue[(radar+1) % 6] &&
			radarValue[radar] >= radarValue[(radar+2) % 6] &&
			radarValue[radar] >= radarValue[(radar+3) % 6] &&
			radarValue[radar] >= radarValue[(radar+4) % 6] &&
			radarValue[radar] >= radarValue[(radar+5) % 6]) {
			return TRUE;
		}
		else {
			return FALSE;
		}
	}



};

typedef struct SongSub {//演奏画面でしか使わない情報

	unsigned short objOfLane[5][4];//レーン毎の要素数
	unsigned short totalMeasures[5];//小節数
}SongSub;




#endif