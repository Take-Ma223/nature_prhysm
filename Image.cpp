#include "Image.h"
#include "DxLib.h"
#include <math.h>


Image::Image(ImageHandle handle, double* time, Cordinate cordinate, BOOL visible, int alpha) : Drawable(time, cordinate, visible, alpha) {
	Image::handle = handle;
}

Image::~Image() {
	//DeleteGraph(handle.getHandle());
}



void Image::draw(int drawScreen) {
	setScreen(drawScreen);
	drawWithProcessing();
}