#pragma once

#include "TransValue.h"
#include "TransAction.h"
#include "Geometry.h"
#include "ImageHandle.h"


struct DrawableInitParam {
	Cordinate cordinate = Cordinate(0, 0);
	BOOL visible = 1;
	int alpha = 255;

	int brightnessR = 255;
	int brightnessG = 255;
	int brightnessB = 255;
};


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

	TransValue brightnessR;
	TransValue brightnessG;
	TransValue brightnessB;


	TransAction action;

	Drawable(double* time = NULL, DrawableInitParam param = DrawableInitParam()) {
		nowTime = time;
		Drawable::visible = TransValue(time);
		Drawable::X = TransValue(time);
		Drawable::Y = TransValue(time);
		Drawable::alpha = TransValue(time);
		Drawable::action = TransAction(time);
	
		Drawable::visible.eSet(param.visible);
		Drawable::X.eSet(param.cordinate.x);
		Drawable::Y.eSet(param.cordinate.y);
		Drawable::alpha.eSet(param.alpha);

		Drawable::brightnessR.eSet(param.brightnessR);
		Drawable::brightnessG.eSet(param.brightnessG);
		Drawable::brightnessB.eSet(param.brightnessB);
	
		Drawable::visible.setValue(param.visible);
		Drawable::X.setValue(param.cordinate.x);
		Drawable::Y.setValue(param.cordinate.y);
		Drawable::alpha.setValue(param.alpha);

		Drawable::brightnessR.setValue(param.brightnessR);
		Drawable::brightnessG.setValue(param.brightnessG);
		Drawable::brightnessB.setValue(param.brightnessB);

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

