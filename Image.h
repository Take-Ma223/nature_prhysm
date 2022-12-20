#include"DxLib.h"
#include"ImageSet.h"
#include "TransValue.h"
#include "TransAction.h"
#include "View.h"

#pragma once



class Image{
	int handle = 0;//読み込んだ画像ハンドル
	double* nowTime;//現在のゲーム時間へのポインタ

public:
	Image(int handle = 0, double* time = NULL, Cordinate cordinate = Cordinate(0, 0), BOOL visible = 1, int alpha = 255);
	~Image();

	TransValue visible;
	TransValue X;//画面上の絶対座標を示す
	TransValue Y;//画面上の絶対座標を示す
	TransValue alpha;
	TransAction action;

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

