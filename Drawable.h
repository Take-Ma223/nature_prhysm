#pragma once

#include "TransValue.h"
#include "TransAction.h"
#include "Geometry.h"
#include "ImageHandle.h"
#include "Asset.h"
#include "STRUCT_OP.h"
#include "Processable.h"
#include "ActivityContext.h"

class ActivityContext;

struct DrawableInitParam {
	Cordinate cordinate = Cordinate(0, 0);
	BOOL visible = 1;
	int alpha = 255;

	int brightnessR = 255;
	int brightnessG = 255;
	int brightnessB = 255;
};


/// <summary>
/// 描画可能なクラスを表す抽象クラス
/// </summary>
class Drawable : public Processable
{
public:

	TransValue visible;
	TransValue X;//描画先の絶対座標を示す
	TransValue Y;//描画先の絶対座標を示す
	TransValue alpha;

	TransValue brightnessR;
	TransValue brightnessG;
	TransValue brightnessB;

	TransAction action;

	double centerRatioX = 0;//左上を(0,0)としたときの中心座標(このDrawable内の割合)
	double centerRatioY = 0;//左上を(0,0)としたときの中心座標(このDrawable内の割合)

	Drawable() {};

	Drawable(ActivityContext* context, DrawableInitParam param = DrawableInitParam()) {
		initParam(context, param);
		setParentHandle(context->getBaseHandle());
	};

	Drawable(Drawable* parent, ActivityContext* context, DrawableInitParam param = DrawableInitParam()) {
		initParam(context, param);
		setParentHandle(parent->handle);
	};

	virtual ~Drawable() {};
	void draw();//毎フレーム呼ぶ
	virtual void draw(int drawScreen) = 0;

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
	ActivityContext* context = NULL;

	ImageHandle handle;//表示する画像ハンドル情報

	void setScreen(int drawScreen);
	
	void drawWithProcessing();

	ImageHandle parentHandle;//このDrawableを描画する対象のハンドル(親がいない場合はDX_SCREEN_BACK)

private:
	void initParam(ActivityContext* context, DrawableInitParam param) {
		Drawable::context = context;
		Drawable::visible = TransValue(context);
		Drawable::X = TransValue(context);
		Drawable::Y = TransValue(context);
		Drawable::alpha = TransValue(context);
		Drawable::action = TransAction(context);

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

