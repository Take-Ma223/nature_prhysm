#include "ActionEvent.h"

ActionEvent::ActionEvent(double timeStart, double timeEnd, std::function<void(void)> handler) : Event(timeStart, timeEnd)
{
	ActionEvent::handler = handler;
}

void ActionEvent::action()
{
	handler();
}
