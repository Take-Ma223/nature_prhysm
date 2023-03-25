#include "Sound.h"
#include "DxLib.h"
#include <math.h>


Sound::Sound(int handle, ActivityContext* c, int x, int y, BOOL visible, int alpha) {
	Sound::visible = TransValue(c);
	Sound::alpha = TransValue(c);

	Sound::handle = handle;

	Sound::visible.eSet(visible);

	Sound::alpha.eSet(alpha);

	Sound::visible.value = visible;
	Sound::alpha.value = alpha;

}

Sound::~Sound() {
	//DeleteGraph(handle);
}

void Sound::clearAllEvent() {
	visible.clearEvent();
	alpha.clearEvent();
}

void Sound::playAll() {
	visible.play();
	alpha.play();
}
void Sound::stopAll() {
	visible.stop();
	alpha.stop();
}
void Sound::resumeAll() {
	visible.resume();
	alpha.resume();
}
void Sound::reverseAll() {
	visible.reverse();
	alpha.reverse();
}
void Sound::setReverseAll(BOOL isReverse) {
	visible.setReverse(isReverse);
	alpha.setReverse(isReverse);
}
void Sound::setLoopAll(BOOL isLoop) {
	visible.setLoop(isLoop);
	alpha.setLoop(isLoop);
}
void Sound::setPlaySpeedAll(double playSpeed) {
	visible.setPlaySpeed(playSpeed);
	alpha.setPlaySpeed(playSpeed);
}

void Sound::draw() {
	visible.process();
	alpha.process();


	SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha.value);
	if (visible.value && alpha.value != 0) {
		DrawGraph(0, 0, handle, TRUE);
	}
}