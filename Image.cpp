#include "Image.h"
#include "DxLib.h"
#include <math.h>

void Image::draw()
{
	Drawable::draw();
}

void Image::draw(int drawScreen) {
	setScreen(drawScreen);
	drawWithProcessing(drawScreen);
}