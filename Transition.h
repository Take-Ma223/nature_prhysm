#pragma once
#include"DxLib.h"
#include <queue>
#include <functional>
#include "Event.h"

class Transition
{
	double* nowTime;//���݂̃Q�[�����Ԃւ̃|�C���^
	
	void calculateWhereToPlay();
	void decideWhichEventToProcess();

	
	double getNowTime();
	int getLastIndex();

protected:
	std::vector<Event*> transition;//�C�x���g�̔z��

	int value = 0;

	double playSpeed = 1;//�Đ����x
	double playStartTime = 0;//�g�����W�V�������̂ǂ̎�������Đ����邩
	double playingTime = 0;//�g�����W�V�������̂ǂ����Đ����Ă��邩
	double durationOffset = 0;//playStartTime + duration���g�����W�V�����̒������z�����Ƃ��ɂ��炩���߃g�����W�V�������������Ĕ͈͓��Ɏ��܂�悤�ɂ���

	double playedTimeOfCaller = 0;//�g�����W�V�������Đ�(�܂��͓r���Đ�)�J�n�����Ƃ��̌Ăяo�����̎���
	int eventIndex = 0;//���������Ă���C�x���g�ԍ�

	BOOL isPlay = FALSE;//�Đ����Ă��邩

	BOOL isLoop = FALSE;//���[�v�Đ����邩�ǂ���
	BOOL isReverse = FALSE;//�t�Đ����邩�ǂ���

	void push(Event*);

	virtual void onPlay(BOOL isReverse) {};
	virtual void onReverse(BOOL isReverse) {};
	virtual void onUpdate() {};
	virtual void onLoop(BOOL isReverse) {};
	virtual void onThrough(BOOL isReverse) {};
	virtual void onNext(BOOL isReverse) {};

public:
	
	Transition(double* time = NULL);
	void update();

	void clearEvent();//transition����ɂ��č폜

	void play();//�ŏ�����Đ�(�t�Đ��̎��͍Ōォ��Đ�)
	void stop();//�Đ���~
	void resume();//�r������Đ�
	void reverse();       //���݂̍Đ��������t�ɂ���
	void setReverse(BOOL);    //�ǂ�������ōĐ����邩�w��
	void setLoop(BOOL);       //���[�v�Đ����邩�ǂ����ݒ�
	void setPlaySpeed(double);//�Đ��{����ݒ�

	void setStartTimeFromTime(int);//�Đ��ʒu��ݒ�(���Ԏw��)
	void setStartTimeFromRange(double);//�Đ��ʒu��ݒ�(�����w��)

};

