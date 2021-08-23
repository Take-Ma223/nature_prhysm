#ifndef _STRUCT_CONFIG
#define _STRUCT_CONFIG

typedef struct CONFIG {//コンフィグデータ
	int FullScreen = 0;
	int Vsync = 1;
	int VsyncOffsetCompensation = 1;
	int Fps = 60;
	int ShowFps = 0;
	int ShowStrShadow = 1;
	int UseHiPerformanceTimer = 1;
	int SongSelectRowNumber = 15;
	int DisplayTimingOffset = 0;//ディスプレイの遅延補正値
	int ShowDebug = 1;//デモモード (1の時にプレイ画面でDEBUG MODEが表示されないようになる)
	int Local = 0;//ソケット通信の接続先IPアドレスをローカルにする
} CONFIG;

#endif