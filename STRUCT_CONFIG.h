#ifndef _STRUCT_CONFIG
#define _STRUCT_CONFIG

enum SoundOutputType {
	DirectSound,
	WASAPI,
	ASIO
};

enum WasapiExclusive {
	NotExclusive,
	Exclusive
};

typedef struct Config {//コンフィグデータ
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
	int UsePy = 0;//utilフォルダのプログラムにpyファイルを使用する

	int SoundOutputType = 0;
	int WasapiExclusive = 0;
	int AsioDriver = 0;
	int Buffer = 256;
	int ComPort = 0;
	int Editable = 0;
} Config;

#endif