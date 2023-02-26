#pragma once
#include "Transition.h"
#include "ActionEvent.h"

class TransAction : public Transition
{
	ActionEvent* actionEvent(Event* _event);//ActionEventå^Ç…ÉLÉÉÉXÉg

protected:
	void onPlay(BOOL isReverse) override;
	void onReverse(BOOL isReverse) override;
	void onUpdate() override;
	void onLoop(BOOL isReverse) override;
	void onThrough(BOOL isReverse) override;
	void onNext(BOOL isReverse) override;


public:
	TransAction(ActivityContext* context = NULL);
	void eSet(double startTime = 0, std::function<void(void)> handler = [] {});

};

