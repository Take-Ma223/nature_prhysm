#include "Image.h"
#include "DxLib.h"
#include <math.h>


Image::Image(int handle, double* time, Cordinate cordinate, BOOL visible, int alpha) {
	Image::X = TransValue(time);
	Image::Y = TransValue(time);
	Image::visible = TransValue(time);
	Image::alpha = TransValue(time);
	Image::action = TransAction(time);

	Image::handle = handle;

	Image::visible.eSet(visible);

	Image::X.eSet(cordinate.x);

	Image::Y.eSet(cordinate.y);

	Image::alpha.eSet(alpha);

	Image::X.setValue(cordinate.x);
	Image::Y.setValue(cordinate.y);
	Image::visible.setValue(visible);
	Image::alpha.setValue(alpha);

}

Image::~Image() {
	//DeleteGraph(handle);
}

void Image::clearAllEvent() {
	visible.clearEvent();
	X.clearEvent();
	Y.clearEvent();
	alpha.clearEvent();
	action.clearEvent();
}

void Image::playAll() {
	visible.play();
	X.play();
	Y.play();
	alpha.play();
	action.play();
}
void Image::stopAll() {
	visible.stop();
	X.stop();
	Y.stop();
	alpha.stop();
	action.stop();
}
void Image::resumeAll() {
	visible.resume();
	X.resume();
	Y.resume();
	alpha.resume();
	action.resume();
}
void Image::reverseAll() {
	visible.reverse();
	X.reverse();
	Y.reverse();
	alpha.reverse();
	action.reverse();
}
void Image::setReverseAll(BOOL isReverse) {
	visible.setReverse(isReverse);
	X.setReverse(isReverse);
	Y.setReverse(isReverse);
	alpha.setReverse(isReverse);
	action.setReverse(isReverse);
}
void Image::setLoopAll(BOOL isLoop) {
	visible.setLoop(isLoop);
	X.setLoop(isLoop);
	Y.setLoop(isLoop);
	alpha.setLoop(isLoop);
	action.setLoop(isLoop);
}
void Image::setPlaySpeedAll(double playSpeed) {
	visible.setPlaySpeed(playSpeed);
	X.setPlaySpeed(playSpeed);
	Y.setPlaySpeed(playSpeed);
	alpha.setPlaySpeed(playSpeed);
	action.setPlaySpeed(playSpeed);
}

void Image::draw() {
	visible.update();
	X.update();
	Y.update();
	alpha.update();
	action.update();

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha.getValue());
	if (visible.getValue() && alpha.getValue() != 0) {
		DrawGraph(X.getValue(), Y.getValue(), handle, TRUE);
	}
}