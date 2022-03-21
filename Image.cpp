#include "Image.h"
#include "DxLib.h"
#include <math.h>


Image::Image(int handle, double* time, int x, int y, BOOL visible, int alpha) {
	Image::X = Animation(time);
	Image::Y = Animation(time);
	Image::visible = Animation(time);
	Image::alpha = Animation(time);

	Image::handle = handle;

	Image::visible.eSet(visible);

	Image::X.eSet(x);

	Image::Y.eSet(y);

	Image::alpha.eSet(alpha);

	Image::X.setValue(x);
	Image::Y.setValue(y);
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
}

void Image::playAll() {
	visible.play();
	X.play();
	Y.play();
	alpha.play();
}
void Image::stopAll() {
	visible.stop();
	X.stop();
	Y.stop();
	alpha.stop();
}
void Image::resumeAll() {
	visible.resume();
	X.resume();
	Y.resume();
	alpha.resume();
}
void Image::reverseAll() {
	visible.reverse();
	X.reverse();
	Y.reverse();
	alpha.reverse();
}
void Image::setReverseAll(BOOL isReverse) {
	visible.setReverse(isReverse);
	X.setReverse(isReverse);
	Y.setReverse(isReverse);
	alpha.setReverse(isReverse);
}
void Image::setLoopAll(BOOL isLoop) {
	visible.setLoop(isLoop);
	X.setLoop(isLoop);
	Y.setLoop(isLoop);
	alpha.setLoop(isLoop);
}
void Image::setPlaySpeedAll(double playSpeed) {
	visible.setPlaySpeed(playSpeed);
	X.setPlaySpeed(playSpeed);
	Y.setPlaySpeed(playSpeed);
	alpha.setPlaySpeed(playSpeed);
}

void Image::draw() {
	visible.update();
	X.update();
	Y.update();
	alpha.update();


	SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha.getValue());
	if (visible.getValue() && alpha.getValue() != 0) {
		DrawGraph(X.getValue(), Y.getValue(), handle, TRUE);
	}
}