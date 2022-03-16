#pragma once
#include"DxLib.h"
#include <queue>

enum ConvertMode {
	Teleportation,
	Linear,
	HalfCosine,
	QuarterSine,
	QuarterCosine,
	Exponential
};

class Converter {
	ConvertMode mode = Linear;
	double paramA = 0;//mode��Exponential�̂Ƃ��̒�

public:
	Converter(ConvertMode mode = Linear, double paramA = 0);
	ConvertMode getMode();
	double getParamA();

};



enum Specify {
	Abs,//���
	Rel//����
};

//�l�̐�Α��Ύw��N���X
class point {
	int val = 0;
	int different = 0;
	Specify specify = Abs;//Rel�̂Ƃ���different�𑊑Βl�Ƃ��Ĉ���

public:
	point(int val = 0, Specify specify = Abs);

	void determinValueFrom(int nowVal, BOOL isReverse = FALSE);//���݂̒l�ɑ��Βl��������A�C�x���g���s���Ɏ��s
	int getVal();
};

class event {//�l�̕ω�����ێ�����N���X


	point start;//���Ύw��̂Ƃ��́A���̃C�x���g���s����startVal�̒l�ɂ��̎��̎��g�̒l�����Z�����
	point end;  //���Ύw��̂Ƃ��́A���̃C�x���g���s����endVal�̒l�ɂ��̎��̎��g�̒l�����Z�����

	Converter converter;

	double timeStart = 0;
	double timeEnd = 0;

public:
	event(point start = point(), point end = point(), Converter converter = Converter(Linear, 0), double timeStart = 0, double  timeEnd = 0);

	double calculateTimeRatio(double now);
	double easing(double input);

	void setTime(int start, int end);

	void determinValueFrom(int nowVal, BOOL isReverse = FALSE);//���݂̒l�ɑ��Βl��������A�C�x���g���s���Ɏ��s

	//getter
	int getStartVal();
	int getEndVal();

	double getStartTime();
	double getEndTime();


};

class eventAbsAbs :public event {





};




class Animation
{
	double* nowTime;//���݂̃Q�[�����Ԃւ̃|�C���^

	std::vector<event> transition;//�C�x���g�̔z��
	int value = 0;

	double playSpeed = 1;//�Đ����x

	double playStartTime = 0;//�A�j���[�V�������̂ǂ̎�������Đ����邩
	double playingTime = 0;//�A�j���[�V�������̂ǂ����Đ����Ă��邩
	double durationOffset = 0;//playStartTime + duration���A�j���[�V�����̒������z�����Ƃ��ɂ��炩���߃A�j���[�V�������������Ĕ͈͓��Ɏ��܂�悤�ɂ���


	double playedTimeOfCaller = 0;//�A�j���[�V�������Đ�(�܂��͓r���Đ�)�J�n�����Ƃ��̌Ăяo�����̎���

	int eventIndex = 0;//���������Ă���C�x���g�ԍ�

	BOOL isPlay = FALSE;//�Đ����Ă��邩

	BOOL isLoop = FALSE;//���[�v�Đ����邩�ǂ���
	BOOL isReverse = FALSE;//�t�Đ����邩�ǂ���
	
	void calculateWhereToPlay();
	void decideWhichEventToProcess();

	int calculateVal(event event);
	double getNowTime();
	void push(event);

	int getLastIndex();
public:
	Animation(double* time = NULL);
	int getValue();
	void setValue(int);//�������ȊO�ł͎g��Ȃ�
	void update();


	void eChange(point start, point end, Converter converter = Converter(Teleportation, 4), double durationTime = 0, double startTime = 0);
	void eChangeTo(point end, Converter converter = Converter(Teleportation, 4), double durationTime = 0, double startTime = 0);
	void eSet(point Abs, double startTime = 0);
	void eON(double startTime = 0);
	void eOFF(double startTime = 0);


	void clearEvent();//transition����ɂ��č폜

	void play();//�ŏ�����Đ�(�t�Đ��̎��͍Ōォ��Đ�)
	void stop();//�Đ���~
	void resume();//�r������Đ�
	void reverse();       //���݂̍Đ��������t�ɂ���
	void setReverse(BOOL);    //�ǂ�������ōĐ����邩�w��
	void setLoop(BOOL);       //���[�v�Đ����邩�ǂ����ݒ�
	void setPlaySpeed(double);//�Đ��{����ݒ�

};

