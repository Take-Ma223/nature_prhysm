#pragma once
#include "Transition.h"
#include "ValueEvent.h"

class TransValue : public Transition
{
	ValueEvent* valueEvent(Event* _event);//ValueEvent型にキャスト
	int calculateVal(ValueEvent* valueEvent);
protected:
	void onPlay(BOOL isReverse) override;
	void onReverse(BOOL isReverse) override;
	void onUpdate() override;
	void onLoop(BOOL isReverse) override;
	void onThrough(BOOL isReverse) override;
	void onNext(BOOL isReverse) override;

public:
	TransValue(double* time = NULL);
	int getValue();
	void setValue(int);//初期化時と、他の変数によって値を決めるときに使う

	void eChange(Point start, Point end, Converter converter = Converter(Teleportation, 4), double startTime = 0, double endTimeInput = 0);
	void eChangeTo(Point end, Converter converter = Converter(Teleportation, 4), double startTime = 0, double endTimeInput = 0);
	void eSet(Point Abs, double startTime = 0);
	void eON(double startTime = 0);
	void eOFF(double startTime = 0);



};

