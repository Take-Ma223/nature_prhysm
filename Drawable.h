#pragma once

#include "TransValue.h"
#include "TransAction.h"
#include "Geometry.h"
#include "ImageHandle.h"
#include "Asset.h"
#include "STRUCT_OP.h"
#include "Processable.h"

/**
* Drawableクラスの前提となるコンテキストクラス
* 画面内で共有されているもの
*/
class DrawableContext {
public:
	Asset* asset;
	Option* option;
	double* time;
	ImageHandle baseHandle = ImageHandle(DX_SCREEN_BACK, Size(1280, 720));

	DrawableContext(Asset* asset = NULL, Option* option = NULL, double* time = NULL) {
		DrawableContext::asset = asset;
		DrawableContext::option = option;
		DrawableContext::time = time;
	};
};

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
class Drawable : public Processable
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

	Drawable() {};

	Drawable(DrawableContext* dc, DrawableInitParam param = DrawableInitParam()) {
		initParam(dc, param);
		setParentHandle(dContext->baseHandle);
	};

	Drawable(Drawable* parent, DrawableContext* dc, DrawableInitParam param = DrawableInitParam()) {
		initParam(dc, param);
		setParentHandle(parent->handle);
	};

	virtual ~Drawable() {};
	void draw();//毎フレーム呼ぶ
	virtual void draw(int drawScreen) = 0;

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


	// Processable を介して継承されました
	virtual void process() override;

protected:
	DrawableContext* dContext = NULL;

	ImageHandle handle;//表示する画像ハンドル情報

	void setScreen(int drawScreen);
	
	void drawWithProcessing();

	ImageHandle parentHandle;//このDrawableを描画する対象のハンドル(親がいない場合はDX_SCREEN_BACK)

private:
	void initParam(DrawableContext* dc, DrawableInitParam param) {
		dContext = dc;
		Drawable::visible = TransValue(dContext->time);
		Drawable::X = TransValue(dContext->time);
		Drawable::Y = TransValue(dContext->time);
		Drawable::alpha = TransValue(dContext->time);
		Drawable::action = TransAction(dContext->time);

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
	}

	void setParentHandle(ImageHandle h) {
		parentHandle = h;
	}

};

