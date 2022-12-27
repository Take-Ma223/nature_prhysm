#include "ImageHandle.h"
#include "DxLib.h"

ImageHandle::ImageHandle()
{
	ImageHandle(0, 0, 0);
}

ImageHandle::ImageHandle(int h, int x, int y)
{
	handle = h;
	sizeX = x;
	sizeY = y;
}

int ImageHandle::getHandle()
{
	return handle;
}

int ImageHandle::getSizeX()
{
	return sizeX;
}

int ImageHandle::getSizeY()
{
	return sizeY;
}
