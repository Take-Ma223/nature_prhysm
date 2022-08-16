#pragma once
#include "Transition.h"
#include "ValueEvent.h"

class TransValue : public Transition
{
	ValueEvent* valueEvent(Event* _event);//ValueEventå^Ç…ÉLÉÉÉXÉg
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
	void eChange(point start, point end, Converter converter = Converter(Teleportation, 4), double startTime = 0, double endTimeInput = 0);
	void eChangeTo(point end, Converter converter = Converter(Teleportation, 4), double startTime = 0, double endTimeInput = 0);
	void eSet(point Abs, double startTime = 0);
	void eON(double startTime = 0);
	void eOFF(double startTime = 0);



};

