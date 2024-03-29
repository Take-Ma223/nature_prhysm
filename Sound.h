#include"DxLib.h"
#include"SoundSet.h"
#include"TransValue.h"

#pragma once



class Sound {
	int handle = 0;//読み込んだサウンドハンドル
	double* nowTime;//現在のゲーム時間へのポインタ

public:
	Sound(int handle = 0, ActivityContext* c = NULL, int x = 0, int y = 0, BOOL visible = 1, int alpha = 255);
	~Sound();

	TransValue visible;
	TransValue alpha;

	void clearAllEvent();
	void playAll();//最初から再生(逆再生の時は最後から再生)
	void stopAll();//再生停止
	void resumeAll();//途中から再生
	void reverseAll();       //現在の再生方向を逆にする
	void setReverseAll(BOOL);    //どちら向きで再生するか指定
	void setLoopAll(BOOL);       //ループ再生するかどうか設定
	void setPlaySpeedAll(double);//再生倍率を設定


	void draw();//毎フレーム呼ぶ

};

