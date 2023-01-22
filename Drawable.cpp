#include "Drawable.h"

void Drawable::setCenterRatio(double x, double y)
{
	centerRatioX = x;
	centerRatioY = y;
}

void Drawable::clearAllEvent() {
	visible.clearEvent();
	X.clearEvent();
	Y.clearEvent();
	alpha.clearEvent();
	action.clearEvent();
}

void Drawable::playAll() {
	visible.play();
	X.play();
	Y.play();
	alpha.play();
	action.play();
}
void Drawable::stopAll() {
	visible.stop();
	X.stop();
	Y.stop();
	alpha.stop();
	action.stop();
}
void Drawable::resumeAll() {
	visible.resume();
	X.resume();
	Y.resume();
	alpha.resume();
	action.resume();
}
void Drawable::reverseAll() {
	visible.reverse();
	X.reverse();
	Y.reverse();
	alpha.reverse();
	action.reverse();
}
void Drawable::setReverseAll(BOOL isReverse) {
	visible.setReverse(isReverse);
	X.setReverse(isReverse);
	Y.setReverse(isReverse);
	alpha.setReverse(isReverse);
	action.setReverse(isReverse);
}
void Drawable::setLoopAll(BOOL isLoop) {
	visible.setLoop(isLoop);
	X.setLoop(isLoop);
	Y.setLoop(isLoop);
	alpha.setLoop(isLoop);
	action.setLoop(isLoop);
}
void Drawable::setPlaySpeedAll(double playSpeed) {
	visible.setPlaySpeed(playSpeed);
	X.setPlaySpeed(playSpeed);
	Y.setPlaySpeed(playSpeed);
	alpha.setPlaySpeed(playSpeed);
	action.setPlaySpeed(playSpeed);
}

void Drawable::setScreen(int drawScreen)
{
	SetDrawScreen(drawScreen);
}

void Drawable::drawWithProcessing()
{
	visible.update();
	X.update();
	Y.update();
	alpha.update();
	action.update();

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha.getValue());
	SetDrawBright(brightnessR.getValue(), brightnessG.getValue(), brightnessB.getValue());
	if (visible.getValue() && alpha.getValue() != 0) {
		DrawGraph(
			X.getValue() - handle.getSize().x * centerRatioX, 
			Y.getValue() - handle.getSize().y * centerRatioY,
			handle.getHandle(),
			TRUE);
	}
}

