#pragma once

/**
* �������ɑ��݂���ꖇ�̉摜�̃n���h������\���N���X 
* 
*/
class ImageHandle {
	int handle;//�摜�n���h��
	int sizeX;
	int sizeY;

public:
	ImageHandle();
	ImageHandle(int handle, int sizeX, int sizeY);

	int getHandle();
	int getSizeX();
	int getSizeY();
};
