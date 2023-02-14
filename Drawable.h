#pragma once

#include "TransValue.h"
#include "TransAction.h"
#include "Geometry.h"
#include "ImageHandle.h"
#include "Asset.h"
#include "STRUCT_OP.h"
#include "Processable.h"

/**
* Drawable�N���X�̑O��ƂȂ�R���e�L�X�g�N���X
* ��ʓ��ŋ��L����Ă������
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
* �`��\�ȃN���X��\�����ۃN���X
**/
class Drawable : public Processable
{
public:

	TransValue visible;
	TransValue X;//��ʏ�̐�΍��W������
	TransValue Y;//��ʏ�̐�΍��W������
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
	void draw();//���t���[���Ă�
	virtual void draw(int drawScreen) = 0;

	double centerRatioX = 0;//�����(0,0)�Ƃ����Ƃ��̒��S���W(����Drawable���̊���)
	double centerRatioY = 0;//�����(0,0)�Ƃ����Ƃ��̒��S���W(����Drawable���̊���)

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
	DrawableContext* dContext = NULL;

	ImageHandle handle;//�\������摜�n���h�����

	void setScreen(int drawScreen);
	
	void drawWithProcessing();

	ImageHandle parentHandle;//����Drawable��`�悷��Ώۂ̃n���h��(�e�����Ȃ��ꍇ��DX_SCREEN_BACK)

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

