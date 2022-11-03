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
	double paramA = 0;//modeがExponentialのときの底

public:
	Converter(ConvertMode mode = Linear, double paramA = 0);
	ConvertMode getMode();
	double getParamA();

};

enum Specify {
	Abs,//絶対
	Rel//相対
};

//値の絶対相対指定クラス
class Point {
	int val = 0;
	int different = 0;
	Specify specify = Abs;//Relのときはdifferentを相対値として扱う

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
	void determinValueFrom(int nowVal, BOOL isReverse = FALSE);//現在の値から実際に遷移する値の初期値、終了値を決める、イベント実行時に実行


	int getStartVal();
	int getEndVal();

	int getStartRelVal();
	int getEndRelVal();

};