#include"DxLib.h"
#include"SoundSet.h"
#include"TransValue.h"

#pragma once



class Sound {
	int handle = 0;//�ǂݍ��񂾃T�E���h�n���h��
	double* nowTime;//���݂̃Q�[�����Ԃւ̃|�C���^

public:
	Sound(int handle = 0, double* time = NULL, int x = 0, int y = 0, BOOL visible = 1, int alpha = 255);
	~Sound();

	TransValue visible;
	TransValue alpha;

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

