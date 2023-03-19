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
	void draw();//���t���[���Ă�
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

