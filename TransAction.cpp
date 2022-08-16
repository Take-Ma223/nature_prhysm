#include "TransAction.h"

//ƒCƒxƒ“ƒg‘}“ü

TransAction::TransAction(double* time) :Transition(time)
{
}

void TransAction::eSet(double startTime, std::function<void(void)> handler) {
	push(
		new ActionEvent(startTime, startTime, handler)
	);
}


ActionEvent* TransAction::actionEvent(Event* _event)
{
	return dynamic_cast<ActionEvent*>(_event);
}


void TransAction::onPlay(BOOL isReverse)
{

	actionEvent(transition[eventIndex])->action();
}

void TransAction::onReverse(BOOL isReverse)
{
}

void TransAction::onUpdate()
{
}

void TransAction::onLoop(BOOL isReverse)
{
	if (!isReverse) {
		actionEvent(transition.back())->action();
		actionEvent(transition[eventIndex])->action();
	}
	else {
		actionEvent(transition.front())->action();
		actionEvent(transition[eventIndex])->action();
	}




}

void TransAction::onThrough(BOOL isReverse)
{
}

void TransAction::onNext(BOOL isReverse)
{
	actionEvent(transition[eventIndex])->action();
}