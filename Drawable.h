#pragma once

#include "TransValue.h"
#include "TransAction.h"
#include "Geometry.h"
#include "ImageHandle.h"

/**
* �`��\�ȃN���X��\�����ۃN���X
**/
class Drawable
{
public:

	TransValue visible;
	TransValue X;//��ʏ�̐�΍��W������
	TransValue Y;//��ʏ�̐�΍��W������
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
	virtual void draw(int drawScreen) = 0;//���t���[���Ă�


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


protected:
	ImageHandle handle;//�\������摜�n���h�����
	double* nowTime;//���݂̃Q�[�����Ԃւ̃|�C���^

	void setScreen(int drawScreen);
	
	void drawWithProcessing();
	virtual int drawHandle() = 0;

};

