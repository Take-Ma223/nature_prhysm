#include "TransValue.h"



TransValue::TransValue(double* time):Transition(time)
{
}

int TransValue::getValue() {
	return value;
}

void TransValue::setValue(int input)
{
	value = input;
	return;
}

//イベント挿入
void TransValue::eChange(Point start, Point end, Converter converter, double startTime, double endTimeInput) {
	double endTime = endTimeInput;

	if (endTime < startTime) {
		endTime = startTime;
	}

	push(
		new ValueEvent(start, end, converter, startTime, endTime)
	);
}
void TransValue::eChangeTo(Point end, Converter converter, double startTime, double endTimeInput) {
	double endTime = endTimeInput;

	if (endTime < startTime) {
		endTime = startTime;
	}

	push(
		new ValueEvent(Point(0, Rel), end, converter, startTime, endTime)
	);
	
}
void TransValue::eSet(Point val, double startTime) {
	push(
		new ValueEvent(val, val, Converter(Teleportation, 1), startTime, startTime)
	);
}
void TransValue::eON(double startTime) {
	push(
		new ValueEvent(1, 1, Converter(Teleportation, 1), startTime, startTime)
	);
}
void TransValue::eOFF(double startTime) {
	push(
		new ValueEvent(0, 0, Converter(Teleportation, 1), startTime, startTime)
	);
}



ValueEvent* TransValue::valueEvent(Event* _event)
{
	ValueEvent* ve = dynamic_cast<ValueEvent*>(_event);
	return ve;
}



int TransValue::calculateVal(ValueEvent* valueEvent) {
	//遷移時間経過割合
	double timeRatio = valueEvent->calculateTimeRatio(playingTime);

	//遷移量にかける値(この値の変化具合で遷移の仕方が変わる)
	double multipleRatio = valueEvent->easing(timeRatio);

	double transitionAmount = (double)valueEvent->getEndVal() - valueEvent->getStartVal();

	return (int)((double)valueEvent->getStartVal() + transitionAmount * multipleRatio);
}

void TransValue::onPlay(BOOL isReverse)
{
	valueEvent(transition[eventIndex])->determinValueFrom(value, isReverse);

}

void TransValue::onReverse(BOOL isReverse)
{
	valueEvent(transition[eventIndex])->determinValueFrom(value, isReverse);

}

void TransValue::onUpdate()
{
	value = calculateVal(valueEvent(transition[eventIndex]));
}

void TransValue::onLoop(BOOL isReverse)
{
	if (!isReverse) {
		valueEvent(transition[eventIndex])->determinValueFrom(valueEvent(transition.back())->getEndVal(), isReverse);
	}
	else {
		valueEvent(transition[eventIndex])->determinValueFrom(valueEvent(transition.front())->getStartVal(), isReverse);
	}

}

void TransValue::onThrough(BOOL isReverse)
{
	if (!isReverse) {
		valueEvent(transition[eventIndex])->determinValueFrom(valueEvent(transition[eventIndex - 1])->getEndVal(), isReverse);//イベントを通ったときに遷移値を決める
	}
	else {
		valueEvent(transition[eventIndex])->determinValueFrom(valueEvent(transition[eventIndex + 1])->getStartVal(), isReverse);
	}


}

void TransValue::onNext(BOOL isReverse)
{
}
