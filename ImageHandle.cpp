#include "ImageHandle.h"
#include "DxLib.h"

ImageHandle::ImageHandle()
{
	ImageHandle(0);
}

ImageHandle::ImageHandle(int h, Size s)
{
	handle = h;
	size = s;
}

int ImageHandle::getHandle()
{
	return handle;
}

Size ImageHandle::getSize()
{
	return size;
}
