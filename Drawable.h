#pragma once

#include "TransValue.h"
#include "TransAction.h"
#include "Geometry.h"
#include "ImageHandle.h"
#include "Asset.h"
#include "STRUCT_OP.h"
#include "Processable.h"
#include "ActivityContext.h"
#include <map>

class ActivityContext;

enum class BlendMode {
	GRAPH_BLEND_NORMAL,
	GRAPH_BLEND_RGBA_SELECT_MIX,
	GRAPH_BLEND_MULTIPLE,
	GRAPH_BLEND_DIFFERENCE,
	GRAPH_BLEND_ADD,
	GRAPH_BLEND_SCREEN,
	GRAPH_BLEND_OVERLAY,
	GRAPH_BLEND_DODGE,
	GRAPH_BLEND_BURN,
	GRAPH_BLEND_DARKEN,
	GRAPH_BLEND_LIGHTEN,
	GRAPH_BLEND_SOFTLIGHT,
	GRAPH_BLEND_HARDLIGHT,
	GRAPH_BLEND_EXCLUSION
};

struct BlendModeParam {
	BlendMode mode = BlendMode::GRAPH_BLEND_NORMAL;
	TransValue blendRatio;

	BlendModeParam(
		BlendMode m = BlendMode::GRAPH_BLEND_NORMAL,
		int br = 255
	) {
		mode = m;
		blendRatio.value = br;
	}

	int convert() {
		map<BlendMode, int> convertMap = {
			{BlendMode::GRAPH_BLEND_NORMAL, DX_GRAPH_BLEND_NORMAL},
			{BlendMode::GRAPH_BLEND_RGBA_SELECT_MIX, DX_GRAPH_BLEND_RGBA_SELECT_MIX},
			{BlendMode::GRAPH_BLEND_MULTIPLE, DX_GRAPH_BLEND_MULTIPLE},
			{BlendMode::GRAPH_BLEND_DIFFERENCE, DX_GRAPH_BLEND_DIFFERENCE},
			{BlendMode::GRAPH_BLEND_ADD, DX_GRAPH_BLEND_ADD},
			{BlendMode::GRAPH_BLEND_SCREEN, DX_GRAPH_BLEND_SCREEN},
			{BlendMode::GRAPH_BLEND_OVERLAY, DX_GRAPH_BLEND_OVERLAY},
			{BlendMode::GRAPH_BLEND_DODGE, DX_GRAPH_BLEND_DODGE},
			{BlendMode::GRAPH_BLEND_BURN, DX_GRAPH_BLEND_BURN},
			{BlendMode::GRAPH_BLEND_DARKEN, DX_GRAPH_BLEND_DARKEN},
			{BlendMode::GRAPH_BLEND_LIGHTEN, DX_GRAPH_BLEND_LIGHTEN},
			{BlendMode::GRAPH_BLEND_SOFTLIGHT, DX_GRAPH_BLEND_SOFTLIGHT},
			{BlendMode::GRAPH_BLEND_HARDLIGHT, DX_GRAPH_BLEND_HARDLIGHT},
			{BlendMode::GRAPH_BLEND_EXCLUSION, DX_GRAPH_BLEND_EXCLUSION}
		};

		return convertMap[mode];
	}
};

struct ExtendParam {
	bool isExtend = false;
	double ratioX = 1;
	double ratioY = 1;

	ExtendParam(
		bool isExtend = false,
		double ratioX = 1,
		double ratioY = 1
	) {
		ExtendParam::isExtend = isExtend;
		ExtendParam::ratioX = ratioX;
		ExtendParam::ratioY = ratioY;
	}
};




/// <summary>
/// Drawableクラスの初期化パラメータ
/// </summary>
struct DrawableInitParam {
	Cordinate cordinate = Cordinate(0, 0);
	CenterRatio centerRatio = CenterRatio(0, 0);
	bool visible = 1;
	int alpha = 255;

	int brightnessR = 255;
	int brightnessG = 255;
	int brightnessB = 255;

	BlendModeParam blendModeParam = BlendModeParam();
	ExtendParam extendParam = ExtendParam();

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="co">座標</param>
	/// <param name="cr">中心座標</param>
	/// <param name="v">可視性</param>
	/// <param name="a">アルファ値</param>
	/// <param name="bR">輝度R</param>
	/// <param name="bG">輝度G</param>
	/// <param name="bB">輝度B</param>
	DrawableInitParam(
		Cordinate co = Cordinate(0, 0),
		CenterRatio cr = CenterRatio(0, 0),
		bool v = TRUE,
		int a = 255,
		int bR = 255,
		int bG = 255,
		int bB = 255,
		BlendModeParam bm = BlendModeParam(),
		ExtendParam ep = ExtendParam()
	) {
		cordinate = co;
		centerRatio = cr;
		visible = v;
		alpha = a;
		brightnessR = bR;
		brightnessG = bG;
		brightnessB = bB;
		blendModeParam = bm;
		extendParam = ep;
	}
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

	BlendModeParam blendModeParam;

	TransAction action;

	double centerRatioX = 0;//左上を(0,0)としたときの中心座標(このDrawable内の割合)
	double centerRatioY = 0;//左上を(0,0)としたときの中心座標(このDrawable内の割合)

	ExtendParam extendParam = ExtendParam();

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
	virtual void draw();//毎フレーム呼ぶ
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

	Size getSize() {
		return handle.getSize();
	}

	int getHandle() {
		return handle.getHandle();
	}

protected:
	ActivityContext* context = NULL;

	ImageHandle handle;//表示する画像ハンドル情報

	int setScreen(int drawScreen);
	
	void drawWithProcessing(int drawScreen);

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

		Drawable::centerRatioX = param.centerRatio.x;
		Drawable::centerRatioY = param.centerRatio.y;

		Drawable::brightnessR.eSet(param.brightnessR);
		Drawable::brightnessG.eSet(param.brightnessG);
		Drawable::brightnessB.eSet(param.brightnessB);

		Drawable::visible.value = param.visible;
		Drawable::X.value = param.cordinate.x;
		Drawable::Y.value = param.cordinate.y;
		Drawable::alpha.value = param.alpha;

		Drawable::brightnessR.value = param.brightnessR;
		Drawable::brightnessG.value = param.brightnessG;
		Drawable::brightnessB.value = param.brightnessB;

		Drawable::blendModeParam = param.blendModeParam;

		Drawable::extendParam = param.extendParam;
	}

	void setParentHandle(ImageHandle h) {
		parentHandle = h;
	}

};

