#pragma once

/**
* メモリに存在する一枚の画像のハンドル情報を表すクラス 
* 
*/
class ImageHandle {
	int handle;//画像ハンドル
	int sizeX;
	int sizeY;

public:
	ImageHandle();
	ImageHandle(int handle, int sizeX, int sizeY);

	int getHandle();
	int getSizeX();
	int getSizeY();
};
