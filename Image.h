#include"DxLib.h"
#include"ImageSet.h"
#include"Animation.h"

#pragma once



class Image{
	int handle = 0;//読み込んだ画像ハンドル
	double* nowTime;//現在のゲーム時間へのポインタ

public:
	Image(int handle = 0, double* time = NULL, int x = 0, int y = 0, BOOL visible = 1, int alpha = 255);
	~Image();

	Animation visible;
	Animation X;
	Animation Y;
	Animation alpha;

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

