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

/// <summary>
/// Drawable�N���X�̏������p�����[�^
/// </summary>
struct DrawableInitParam {
	Cordinate cordinate = Cordinate(0, 0);
	bool visible = 1;
	int alpha = 255;

	int brightnessR = 255;
	int brightnessG = 255;
	int brightnessB = 255;

	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	/// <param name="c">���W</param>
	/// <param name="v">����</param>
	/// <param name="a">�A���t�@�l</param>
	/// <param name="bR">�P�xR</param>
	/// <param name="bG">�P�xG</param>
	/// <param name="bB">�P�xB</param>
	DrawableInitParam(
		Cordinate c = Cordinate(0, 0),
		bool v = TRUE,
		int a = 255,
		int bR = 255,
		int bG = 255,
		int bB = 255
	) {
		cordinate = c;
		visible = v;
		alpha = a;
		brightnessR = bR;
		brightnessG = bG;
		brightnessB = bB;
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

	TransAction action;

	double centerRatioX = 0;//�����(0,0)�Ƃ����Ƃ��̒��S���W(����Drawable���̊���)
	double centerRatioY = 0;//�����(0,0)�Ƃ����Ƃ��̒��S���W(����Drawable���̊���)

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

protected:
	ActivityContext* context = NULL;

	ImageHandle handle;//�\������摜�n���h�����

	void setScreen(int drawScreen);
	
	void drawWithProcessing();

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
	}

	void setParentHandle(ImageHandle h) {
		parentHandle = h;
	}

};

