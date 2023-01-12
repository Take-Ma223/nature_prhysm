#pragma once

#include"DxLib.h"
#include"ImageSet.h"
#include "TransValue.h"
#include "TransAction.h"
#include "View.h"
#include "Drawable.h"
#include "ImageHandle.h"
#include "Geometry.h"


class Image: public Drawable {
	ImageHandle handle;//�\������摜�n���h�����
	double* nowTime;//���݂̃Q�[�����Ԃւ̃|�C���^

public:
	Image(ImageHandle handle = ImageHandle(), double* time = NULL, Cordinate cordinate = Cordinate(0, 0), BOOL visible = 1, int alpha = 255);
	~Image();

	void draw(int drawScreen) override;//���t���[���Ă�
	int drawHandle() override {
		return handle.getHandle();
	};

};

