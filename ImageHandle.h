#pragma once
#include "Geometry.h"

/// <summary>
/// �������ɑ��݂���ꖇ�̉摜�̃n���h������\���N���X 
/// </summary>
class ImageHandle {
	int handle;//�摜�n���h��
	Size size;

public:
	ImageHandle();
	ImageHandle(int handle, Size size = Size(1, 1));

	int getHandle();
	Size getSize();;
};
