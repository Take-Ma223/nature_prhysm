#include "Drawable.h"

void Drawable::draw()
{
	draw(parentHandle.getHandle());
}

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

int Drawable::setScreen(int drawScreen)
{
	return SetDrawScreen(drawScreen);
}

void Drawable::drawWithProcessing(int drawScreen)
{
	visible.process();
	X.process();
	Y.process();
	alpha.process();
	action.process();

	SetDrawMode(DX_DRAWMODE_BILINEAR);
	SetDrawBlendMode(DX_BLENDMODE_PMA_ALPHA, alpha.value);
	SetDrawBright(brightnessR.value, brightnessG.value, brightnessB.value);
	if (visible.value && alpha.value != 0) {
		if (blendModeParam.mode != BlendMode::GRAPH_BLEND_NORMAL) {
			GraphBlend(drawScreen, handle.getHandle(), blendModeParam.blendRatio.value, blendModeParam.convert());
		}
		else if (extendParam.isExtend) {
			auto start_to_center = handle.getSize().x * centerRatioX;
			auto top_to_center = handle.getSize().y * centerRatioY;
			auto end_to_center = handle.getSize().x * centerRatioX;
			auto bottom_to_center = handle.getSize().y * centerRatioY;

			auto x1 = X.value - start_to_center * extendParam.ratioX;
			auto y1 = Y.value - top_to_center * extendParam.ratioY;
			auto x2 = X.value + end_to_center * extendParam.ratioX;
			auto y2 = Y.value + bottom_to_center * extendParam.ratioY;

			DrawExtendGraphF(
				x1,y1,x2,y2,
				handle.getHandle(),
				TRUE);
		}
		else {
			auto x1 = X.value - handle.getSize().x * centerRatioX;
			auto y1 = Y.value - handle.getSize().y * centerRatioY;

			DrawGraph(
				x1,
				y1,
				handle.getHandle(),
				TRUE);
		}
	}

	//��Z�ς݃A���t�@���猳�ɖ߂�
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha.value);
}

void Drawable::process()
{
	draw();
}

