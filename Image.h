#include"DxLib.h"
#include"ImageSet.h"
#include"Animation.h"

#pragma once



class Image{
	int handle = 0;//�ǂݍ��񂾉摜�n���h��
	double* nowTime;//���݂̃Q�[�����Ԃւ̃|�C���^

public:
	Image(int handle = 0, double* time = NULL, int x = 0, int y = 0, BOOL visible = 1, int alpha = 255);
	~Image();

	Animation visible;
	Animation X;
	Animation Y;
	Animation alpha;

	void clearAllEvent();
	void playAll();//�ŏ�����Đ�(�t�Đ��̎��͍Ōォ��Đ�)
	void stopAll();//�Đ���~
	void resumeAll();//�r������Đ�
	void reverseAll();       //���݂̍Đ��������t�ɂ���
	void setReverseAll(BOOL);    //�ǂ�������ōĐ����邩�w��
	void setLoopAll(BOOL);       //���[�v�Đ����邩�ǂ����ݒ�
	void setPlaySpeedAll(double);//�Đ��{����ݒ�


	void draw();//���t���[���Ă�

};

