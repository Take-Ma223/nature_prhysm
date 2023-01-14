#include "Image.h"
#include "DxLib.h"
#include <math.h>

void Image::draw(int drawScreen) {
	setScreen(drawScreen);
	drawWithProcessing();
}