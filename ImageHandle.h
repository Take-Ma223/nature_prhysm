#pragma once
#include "Geometry.h"

/**
* メモリに存在する一枚の画像のハンドル情報を表すクラス 
* 
*/
class ImageHandle {
	int handle;//画像ハンドル
	Size size;

public:
	ImageHandle();
	ImageHandle(int handle, Size size = Size(1, 1));

	int getHandle();
	Size getSize();;
};
