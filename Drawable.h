#pragma once

#include "TransValue.h"
#include "TransAction.h"
#include "Geometry.h"
#include "ImageHandle.h"

/**
* 描画可能なクラスを表す抽象クラス
**/
class Drawable
{
public:

	TransValue visible;
	TransValue X;//画面上の絶対座標を示す
	TransValue Y;//画面上の絶対座標を示す
	TransValue alpha;
	TransAction action;

	Drawable(double* time = NULL, Cordinate cordinate = Cordinate(0, 0), BOOL visible = 1, int alpha = 255) {
		nowTime = time;
		Drawable::visible = TransValue(time);
		Drawable::X = TransValue(time);
		Drawable::Y = TransValue(time);
		Drawable::alpha = TransValue(time);
		Drawable::action = TransAction(time);
	
		Drawable::visible.eSet(visible);
		Drawable::X.eSet(cordinate.x);
		Drawable::Y.eSet(cordinate.y);
		Drawable::alpha.eSet(alpha);
	
		Drawable::X.setValue(cordinate.x);
		Drawable::Y.setValue(cordinate.y);
		Drawable::visible.setValue(visible);
		Drawable::alpha.setValue(alpha);
	};
	virtual ~Drawable() {};
	virtual void draw(int drawScreen) = 0;//毎フレーム呼ぶ


	double centerRatioX = 0;//左上を(0,0)としたときの中心座標(このDrawable内の割合)
	double centerRatioY = 0;//左上を(0,0)としたときの中心座標(このDrawable内の割合)

	void setCenterRatio(double x, double y);

	void clearAllEvent();
	void playAll();//最初から再生(逆再生の時は最後から再生)
	void stopAll();//再生停止
	void resumeAll();//途中から再生
	void reverseAll();       //現在の再生方向を逆にする
	void setReverseAll(BOOL);    //どちら向きで再生するか指定
	void setLoopAll(BOOL);       //ループ再生するかどうか設定
	void setPlaySpeedAll(double);//再生倍率を設定


protected:
	ImageHandle handle;//表示する画像ハンドル情報
	double* nowTime;//現在のゲーム時間へのポインタ

	void setScreen(int drawScreen);
	
	void drawWithProcessing();
	virtual int drawHandle() = 0;

};

