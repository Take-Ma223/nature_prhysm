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
/// Drawable�N���X�̏������p�����[�^
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
	/// �R���X�g���N�^
	/// </summary>
	/// <param name="co">���W</param>
	/// <param name="cr">���S���W</param>
	/// <param name="v">����</param>
	/// <param name="a">�A���t�@�l</param>
	/// <param name="bR">�P�xR</param>
	/// <param name="bG">�P�xG</param>
	/// <param name="bB">�P�xB</param>
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
/// �`��\�ȃN���X��\�����ۃN���X
/// </summary>
class Drawable : public Processable
{
public:

	TransValue visible;
	TransValue X;//�`���̐�΍��W������
	TransValue Y;//�`���̐�΍��W������
	TransValue alpha;

	TransValue brightnessR;
	TransValue brightnessG;
	TransValue brightnessB;

	BlendModeParam blendModeParam;

	TransAction action;

	double centerRatioX = 0;//�����(0,0)�Ƃ����Ƃ��̒��S���W(����Drawable���̊���)
	double centerRatioY = 0;//�����(0,0)�Ƃ����Ƃ��̒��S���W(����Drawable���̊���)

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
	virtual void draw();//���t���[���Ă�
	virtual void draw(int drawScreen) = 0;

	void setCenterRatio(double x, double y);

	void clearAllEvent();
	void playAll();//�ŏ�����Đ�(�t�Đ��̎��͍Ōォ��Đ�)
	void stopAll();//�Đ���~
	void resumeAll();//�r������Đ�
	void reverseAll();       //���݂̍Đ��������t�ɂ���
	void setReverseAll(BOOL);    //�ǂ�������ōĐ����邩�w��
	void setLoopAll(BOOL);       //���[�v�Đ����邩�ǂ����ݒ�
	void setPlaySpeedAll(double);//�Đ��{����ݒ�

	// Processable ����Čp������܂���
	virtual void process() override;

	Size getSize() {
		return handle.getSize();
	}

	int getHandle() {
		return handle.getHandle();
	}

protected:
	ActivityContext* context = NULL;

	ImageHandle handle;//�\������摜�n���h�����

	int setScreen(int drawScreen);
	
	void drawWithProcessing(int drawScreen);

	ImageHandle parentHandle;//����Drawable��`�悷��Ώۂ̃n���h��(�e�����Ȃ��ꍇ��DX_SCREEN_BACK)

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

