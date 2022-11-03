#pragma once
#include"Event.h"
#include"DxLib.h"

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
class Point {
	int val = 0;
	int different = 0;
	Specify specify = Abs;//Rel�̂Ƃ���different�𑊑Βl�Ƃ��Ĉ���

public:
	Point(int val = 0, Specify specify = Abs);

	int updateVal();
	Specify getSpecify();
};

enum EventType {
	AbsAbs,
	AbsRel,
	RelAbs,
	RelRel,
};

class ValueEvent : public Event {
	int startValue = 0;
	int endValue = 0;

	int startRelativeValue = 0;
	int endRelativeValue = 0;

	EventType eventType;

	Converter converter;

	EventType determinEventType(Point, Point);
public:
	ValueEvent(Point start = Point(), Point end = Point(), Converter converter = Converter(Linear, 0), double timeStart = 0, double  timeEnd = 0);


	double easing(double input);
	void determinValueFrom(int nowVal, BOOL isReverse = FALSE);//���݂̒l������ۂɑJ�ڂ���l�̏����l�A�I���l�����߂�A�C�x���g���s���Ɏ��s


	int getStartVal();
	int getEndVal();

	int getStartRelVal();
	int getEndRelVal();

};